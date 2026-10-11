/*
 * STM32F103 bxCAN 드라이버
 * - Pin   : CAN_RX = PB8 (아두이노 헤더 D15), CAN_TX = PB9 (아두이노 헤더 D14); AFIO 리맵 2
 * - Clock : PCLK1(APB1) = 32 MHz  (HSI/2 x16 = 64 MHz SYSCLK, APB1 = /2)
 * - Baud  : 500 kbit/s
 */

#include "BspCan.h"

#include "Kernel.h"
#include "Semaphore.h"
#include "stm32f1xx_hal.h"

// 비트 타이밍(BTR에는 -1한 값을 저장함)
// CAN 클럭 32MHz, 통신속도 500kbps = 1비트당 64번 진동
// BRP = 진동을 몇 개씩 묶어 1tq(time quantum)로 할 지
#define CAN_BRP 4U // 1비트당 16개의 tq로 나눔(4 * 16 = 64); bxCAN에선 8-25개 사이로만 설정 가능
// 1tq = BRP(4) / PCLK1(32MHz) = 125ns; 1bit = 125ns * 16 = 2000ns
//                                       ▼ sampling point
// 1bit = |SYNC|  PROP_SEG  | PHASE_SEG1 | PHASE_SEG2 |
//        |1tq |<          TS1          >|<    TS2   >|
#define CAN_TS1 13U // 읽기 전 tq 수
#define CAN_TS2 2U  // 읽은 후 tq 수
// sampling point = (SYNC(1tq) + TS1(13tq)) / total(16tq) = 14tq / 16tq = 87.5%
#define CAN_SJW 1U // Synchronization Jump Width

// 커널 init 전에 CAN 초기화 호출되므로 반복 횟수로 타임아웃 측정
#define CAN_INIT_TIMEOUT     100000U
#define CAN_RX_BUF_SIZE      16U

typedef struct {
    CanFrame_t        buf[CAN_RX_BUF_SIZE];
    volatile uint16_t head; // write index (ISR 단일 생산)
    volatile uint16_t tail; // read index  (Task 단일 소비)
} CanRxRing;

static CanRxRing         g_can_rx;
static SemHandle_t       g_can_rx_sem = -1; // 수신 프레임 도착 알림용
static volatile uint32_t g_can_rx_drop;     // 링버퍼 full 또는 HW FIFO 오버런으로 버린 프레임 수

// Initialization Acknowledge(INAK)이 expect_set이 될 때까지 대기
static int32_t sWaitInak(uint32_t expect_set) // 1: 초기화 모드 진입 완료 대기; 0: 해제 완료 대기
{
    for (uint32_t i = 0; i < CAN_INIT_TIMEOUT; i++)
    {
        uint32_t inak = (CAN1->MSR & CAN_MSR_INAK) ? 1U : 0U;
        if (inak == expect_set) return 0;
    }
    return -1;
}

// CAN 핀 설정; NUCLEO 아두이노 헤더에서 쓰기 쉽도록 PB8/PB9로 remap; 기본 핀은 PA11/PA12
static void sGpioInit(void)
{
    GPIO_InitTypeDef gpio = {0};

    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_AFIO_CLK_ENABLE();
    __HAL_AFIO_REMAP_CAN1_2();      // CAN_RX -> PB8, CAN_TX -> PB9

    gpio.Pin  = GPIO_PIN_8;         // CAN_RX
    gpio.Mode = GPIO_MODE_INPUT;
    gpio.Pull = GPIO_PULLUP;        // 트랜시버가 없거나 끊겨도 리세시브(1)로 보이게 해서 초기화 해제 시 리세시브 11비트 조건을 만족시키고 쓰레기 값 수신을 막기 위함
    HAL_GPIO_Init(GPIOB, &gpio);

    gpio.Pin   = GPIO_PIN_9;        // CAN_TX
    gpio.Mode  = GPIO_MODE_AF_PP;
    gpio.Pull  = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &gpio);
}

// 수신 필터 설정; 모든 ID 허용
static void sFilterAcceptAll(void)
{
    CAN1->FMR  |= CAN_FMR_FINIT;       // 필터 설정 모드 진입
    CAN1->FA1R &= ~CAN_FA1R_FACT0;     // 설정 중에는 필터 0 비활성화
    CAN1->FS1R |= CAN_FS1R_FSC0;       // 32비트 스케일
    CAN1->FM1R &= ~CAN_FM1R_FBM0;      // 마스크 모드 (1이면 ID 리스트 모드)
    CAN1->FFA1R &= ~CAN_FFA1R_FFA0;    // 통과한 프레임은 FIFO0으로
    CAN1->sFilterRegister[0].FR1 = 0U; // 비교할 ID
    CAN1->sFilterRegister[0].FR2 = 0U; // 마스크 (0 = 비교 안 함)
    CAN1->FA1R |= CAN_FA1R_FACT0;      // 필터 0 활성화
    CAN1->FMR  &= ~CAN_FMR_FINIT;      // 필터 설정 모드 해제
}

// bxCAN 초기화
// bxCAN 설정 레지스터(BTR 등)는 초기화 모드에서만 쓸 수 있음
int32_t BSP_CAN_Init(BspCanMode_t mode)
{
    g_can_rx.head = 0;
    g_can_rx.tail = 0;
    g_can_rx_drop = 0;
    g_can_rx_sem  = Semaphore_Create(0);

    sGpioInit();
    __HAL_RCC_CAN1_CLK_ENABLE(); // APB1에서 CAN1 클럭 공급

    CAN1->MCR &= ~CAN_MCR_SLEEP; // wake; 리셋 직후 bxCAN은 슬립 모드
    CAN1->MCR |= CAN_MCR_INRQ;   // Initialization Request
    if (sWaitInak(1U) < 0) // INAK check
    {
        return -1; // 초기화 모드 진입 실패
    }

    // MCR 동작 옵션
    // ABOM: 오류가 누적되어 bus-off가 되면 하드웨어가 자동 복구
    // TXFP: 송신 메일박스 3개를 요청한 순서대로 전송; 0이면 ID 우선순위 순
    CAN1->MCR |= CAN_MCR_ABOM | CAN_MCR_TXFP;

    // TTCM: 시간 트리거 통신 모드 끔
    // AWUM: 버스 활동으로 자동 웨이크업 끔
    // NART: 0 = 전송 실패 시 자동 재전송 사용
    // RFLM: 0 = FIFO가 가득 차면 가장 오래된 프레임을 새 프레임으로 덮어씀(오버런 플래그 설정)
    CAN1->MCR &= ~(CAN_MCR_TTCM | CAN_MCR_AWUM | CAN_MCR_NART | CAN_MCR_RFLM);

    // 비트 타이밍은 -1한 값 저장
    uint32_t btr = ((CAN_SJW - 1U) << CAN_BTR_SJW_Pos)
                 | ((CAN_TS2 - 1U) << CAN_BTR_TS2_Pos)
                 | ((CAN_TS1 - 1U) << CAN_BTR_TS1_Pos)
                 | (CAN_BRP - 1U);
    if (mode == BSP_CAN_MODE_LOOPBACK)
    {
        // Loopback Mode
        // LBKM: RX 핀 무시; 송신 프레임을 내부에서 바로 수신 쪽으로 되돌림
        // SILM: TX 핀으로 아무것도 내보내지 않음
        btr |= CAN_BTR_LBKM | CAN_BTR_SILM;
    }
    CAN1->BTR = btr;

    sFilterAcceptAll(); // 수신 필터 설정; 모든 ID 허용

    // FIFO0에 메시지가 들어오면(FMP0 > 0) 인터럽트 발생
    CAN1->IER |= CAN_IER_FMPIE0;

    // UART 수신 인터럽트와 같은 우선순위로 설정
    // STM32F103에서는 CAN RX0 인터럽트가 USB 저우선순위 인터럽트와 벡터를 공유함
    HAL_NVIC_SetPriority(USB_LP_CAN1_RX0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);

    // 초기화 모드 해제
    // 일반 모드에서는 버스에서 리세시브 비트(1)가 11개 연속으로 관측되어야 INAK가 0으로 내려감
    CAN1->MCR &= ~CAN_MCR_INRQ;
    if (sWaitInak(0U) < 0)
    {
        return -2; // 초기화 모드 해제 실패
    }
    return 0;
}

// 프레임 송신
// bxCAN에는 송신 메일박스가 3개 있음
// TSR.TMEx(Transmit Mailbox Empty)로 빈 칸을 찾아 ID, 길이, 데이터를 채운 뒤 TIR.TXRQ로 전송을 요청
// 반환값: 사용한 메일박스 번호(0-2), 모두 사용 중이면 -1
int32_t BSP_CAN_Transmit(const CanFrame_t* frame)
{
    uint32_t tsr = CAN1->TSR;
    uint32_t mbx;

    if (tsr & CAN_TSR_TME0)      mbx = 0U;
    else if (tsr & CAN_TSR_TME1) mbx = 1U;
    else if (tsr & CAN_TSR_TME2) mbx = 2U;
    else return -1; // 세 메일박스 모두 전송 대기 중

    uint8_t dlc = (frame->dlc > 8U) ? 8U : frame->dlc;
    CAN_TxMailBox_TypeDef* mb = &CAN1->sTxMailBox[mbx];

    // TIR 레이아웃
    // 표준 ID(11비트): [31:21] STID
    // 확장 ID(29비트): [31:3]  EXID, IDE 비트로 확장 프레임 표시
    // RTR 비트: 리모트 프레임(데이터 없이 상대에게 데이터를 요청)
    uint32_t tir = frame->ext ? (((frame->id & 0x1FFFFFFFU) << 3) | CAN_TI0R_IDE)
                              : ((frame->id & 0x7FFU) << 21);
    if (frame->rtr)
    {
        tir |= CAN_TI0R_RTR;
    }

    // 데이터 8바이트는 하위 4바이트(TDLR)와 상위 4바이트(TDHR)로 나눠 little endian으로 넣음
    mb->TDTR = dlc;
    mb->TDLR = (uint32_t)frame->data[0]       | ((uint32_t)frame->data[1] << 8)
             | ((uint32_t)frame->data[2] << 16) | ((uint32_t)frame->data[3] << 24);
    mb->TDHR = (uint32_t)frame->data[4]       | ((uint32_t)frame->data[5] << 8)
             | ((uint32_t)frame->data[6] << 16) | ((uint32_t)frame->data[7] << 24);

    // TXRQ 비트를 쓰는 순간 하드웨어가 전송을 시작
    mb->TIR  = tir | CAN_TI0R_TXRQ;

    return (int32_t)mbx;
}

// 태스크가 링버퍼에서 프레임 하나를 소비
int32_t BSP_CAN_GetFrame(CanFrame_t* out)
{
    if (g_can_rx.head == g_can_rx.tail)
    {
        return 0; // empty
    }
    *out = g_can_rx.buf[g_can_rx.tail];
    g_can_rx.tail = (uint16_t)((g_can_rx.tail + 1U) % CAN_RX_BUF_SIZE);
    return 1;
}

// 프레임이 올 때까지 block
int32_t BSP_CAN_GetFrameBlocking(CanFrame_t* out)
{
    for (;;)
    {
        if (BSP_CAN_GetFrame(out))
        {
            return 1;
        }
        Semaphore_Wait(g_can_rx_sem);
    }
}

uint32_t BSP_CAN_GetRxDropCount(void)
{
    return g_can_rx_drop;
}

// ISR이 링버퍼에 프레임을 넣고 태스크를 깨움
static void sRxPutFromISR(const CanFrame_t* f)
{
    uint16_t next = (uint16_t)((g_can_rx.head + 1U) % CAN_RX_BUF_SIZE);
    if (next == g_can_rx.tail)
    {
        // full -> drop
        g_can_rx_drop++;
        return;
    }
    g_can_rx.buf[g_can_rx.head] = *f;
    g_can_rx.head = next;
    Kernel_Semaphore_Signal(g_can_rx_sem);
}

// CAN 수신 FIFO0 인터럽트 핸들러
// STM32F103에서는 CAN RX0 인터럽트가 USB 저우선순위 인터럽트와 벡터를 공유함
void USB_LP_CAN1_RX0_IRQHandler(void)
{
    // FIFO0은 3단 깊이라 한 번의 인터럽트에 여러 프레임이 쌓여 있을 수 있으므로
    // FMP0(쌓인 개수)이 0이 될 때까지 모두 비움
    while ((CAN1->RF0R & CAN_RF0R_FMP0) != 0U)
    {
        CAN_FIFOMailBox_TypeDef* mb = &CAN1->sFIFOMailBox[0];
        CanFrame_t f;
        uint32_t rir = mb->RIR;

        // 송신 TIR과 동일 레이아웃: 표준 ID [31:21], 확장 ID [31:3]
        f.ext = (rir & CAN_RI0R_IDE) != 0U;
        f.rtr = (rir & CAN_RI0R_RTR) != 0U;
        f.id  = f.ext ? ((rir >> 3) & 0x1FFFFFFFU) : ((rir >> 21) & 0x7FFU);
        f.dlc = (uint8_t)(mb->RDTR & CAN_RDT0R_DLC);
        if (f.dlc > 8U)
        {
            f.dlc = 8U; // DLC 9~15는 클래식 CAN에서 8바이트로 취급
        }

        uint32_t lo = mb->RDLR;
        uint32_t hi = mb->RDHR;
        for (uint32_t i = 0; i < 4U; i++)
        {
            f.data[i]     = (uint8_t)(lo >> (8U * i));
            f.data[i + 4] = (uint8_t)(hi >> (8U * i));
        }

        // RFOM0: 출력 메일박스를 다 읽었다고 하드웨어에 알려 다음 프레임을 앞으로 당김; 생략시 같은 프레임을 계속 읽게됨
        // RF0R의 FULL0/FOVR0은 1을 쓰면 지워지는(rc_w1) 비트라, |= 로 읽고 다시 쓰면 의도치 않게 플래그가 지워질 수 있어 원하는 비트만 대입
        CAN1->RF0R = CAN_RF0R_RFOM0;
        sRxPutFromISR(&f);
    }

    // 하드웨어 FIFO 오버런: ISR이 늦어 3단 FIFO가 넘친 경우
    if (CAN1->RF0R & CAN_RF0R_FOVR0)
    {
        g_can_rx_drop++;
        CAN1->RF0R = CAN_RF0R_FOVR0; // 1을 써서 플래그 해제
    }
}
