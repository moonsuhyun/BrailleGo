#ifndef KERNEL_KERNEL_H_
#define KERNEL_KERNEL_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

enum
{
    SCG_KERNEL   = 0x0000U,
    SCG_TASK     = 0x0100U,
    SCG_SYSTICK  = 0x0200U,
    SCG_SYNC     = 0x0300U
};

typedef enum SysCallNo
{
    SC_INIT      = SCG_KERNEL  | 0x0001U,
    SC_START     = SCG_KERNEL  | 0x0002U,
    SC_ISRUN     = SCG_KERNEL  | 0x0003U,

    SC_CREATE    = SCG_TASK    | 0x0001U,
    SC_YIELD     = SCG_TASK    | 0x0002U,
    SC_DELAY     = SCG_TASK    | 0x0003U,
    SC_TERMINATE = SCG_TASK    | 0x0004U,

    SC_GETTICK   = SCG_SYSTICK | 0x0001U,

    SC_MUTLOCK   = SCG_SYNC    | 0x0001U,
    SC_MUTUNLOCK = SCG_SYNC    | 0x0002U,
    SC_SEMCREATE = SCG_SYNC    | 0x0003U,
    SC_SEMWAIT   = SCG_SYNC    | 0x0004U,
    SC_SEMSIGNAL = SCG_SYNC    | 0x0005U,
    SC_SEMGETCNT = SCG_SYNC    | 0x0006U

} SysCallNo_t;

typedef enum MutexType
{
    MUTEX_UART,
    MUTEX_TYPE_NUM
} MutexType_t;

typedef int32_t SemHandle_t;

typedef struct
{
    uint32_t id;      // 11-bit (standard) or 29-bit (extended) identifier
    uint8_t  dlc;     // 0..8
    uint8_t  data[8];
    bool     ext;     // extended (29-bit) frame
    bool     rtr;     // remote frame
} CanFrame_t;

typedef void (*KernelTaskFunc_t)(void*);

void Kernel_Start(KernelTaskFunc_t init_task);
int32_t Task_Create(void (*start_func)(void*), void* arg, uint32_t priority); // 태스크 ID, 실패 시 음수
void Task_Yield(void);
void Task_Delay(uint32_t ms);
void Task_Terminate();

int32_t Mutex_Lock(MutexType_t mutex_type);   // 1: 즉시 획득, 0: 대기 후 획득, -EDEADLK: Deadlock 감지
int32_t Mutex_Unlock(MutexType_t mutex_type); // 1: 해제, 0: 소유자 아님

SemHandle_t Semaphore_Create(int32_t initial_count); // 핸들, 실패 시 음수
int32_t Semaphore_Wait(SemHandle_t h);               // 처리 후 카운트, 실패 시 음수
int32_t Semaphore_Signal(SemHandle_t h);
int32_t Semaphore_GetCount(SemHandle_t h);

uint32_t SysTick_GetTick(void);
bool Kernel_Is_Running(void);

int32_t UART_Read(uint8_t* buf, uint32_t len);
int32_t UART_ReadLine(char* buf, uint32_t max_len);
int32_t UART_ReadLineEx(char* buf, uint32_t max_len, void begin(void), void end(void));

int32_t  CAN_Send(CanFrame_t const* frame);    // 사용한 메일박스 번호(0-2), 실패 시 음수
int32_t  CAN_ReceiveBlocking(CanFrame_t* out); // 프레임이 올 때까지 블록
int32_t  CAN_GetInitStatus(void);              // 0 성공, 음수는 BSP_CAN_Init 오류 코드
uint32_t CAN_GetRxDropCount(void);             // 버퍼 부족으로 버린 수신 프레임 수
uint32_t CAN_GetPendingCount(void);            // 송수신 대기 중인 프레임 수

#ifdef __cplusplus
}
#endif

#endif // KERNEL_KERNEL_H_
