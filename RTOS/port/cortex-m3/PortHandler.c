/*
 * PortHandler.c
 *
 *  Created on: May 26, 2025
 *      Author: moon
 */

#ifdef __cplusplus
extern "C" {

#endif

/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f1xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "PortHandler.h"
#include "Kernel.h"
#include "TaskManager.h"
#include "Types.h"
#include "Mutex.h"
#include "Semaphore.h"
#include <stdio.h>
#include <sys/errno.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
// naked 핸들러에서 호출되는 C 헬퍼
static uint32_t sSVC_GetStartTaskSp(void);
static uint32_t sPendSV_SwitchContext(uint32_t psp);
static void     sSVC_SystemCall(int32_t* sp);
// fault dump; 호출 이후 Handler로 복귀하지 않음
static void     sFaultDump(uint32_t* sp);
static void     sStackOverflow_Dump(uint32_t* sp);
// dump 보조 함수
static void     prflag(uint32_t cond, char const* name);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
// SVC#0: 태스크 시작
static __attribute__((used))
uint32_t sSVC_GetStartTaskSp(void)
{
    KernelTcb_t const* tcb = TaskManager_Get_Current_Task();
    return (uint32_t)tcb->sp;
}

// SVC#1: 커널의 시스템 콜 호출
static __attribute__((used))
void sSVC_SystemCall(int32_t* sp)
{
    // arguments
    int32_t const sysno = sp[0];
    int32_t const arg0  = sp[1];
    int32_t const arg1  = sp[2];
    int32_t const arg2  = sp[3];

    int32_t ret;

    switch (sysno)
    {
    case SC_CREATE:
        ret = TaskManager_Create((KernelTaskFunc_t)arg0, (void*)arg1, arg2);
        break;
    case SC_YIELD:
        ret = TaskManager_Yield();
        break;
    case SC_DELAY:
        ret = TaskManager_Delay(arg0);
        break;
    case SC_TERMINATE:
        ret = TaskManager_Terminate();
        break;
    case SC_MUTLOCK:
        ret = Kernel_Mutex_Lock(arg0);
        break;
    case SC_MUTUNLOCK:
        ret = Kernel_Mutex_Unlock(arg0);
        break;
    case SC_SEMCREATE:
        ret = Kernel_Semaphore_Create(arg0);
        break;
    case SC_SEMWAIT:
        ret = Kernel_Semaphore_Wait(arg0);
        break;
    case SC_SEMSIGNAL:
        ret = Kernel_Semaphore_Signal(arg0);
        break;
    case SC_SEMGETCNT:
        ret = Kernel_Semaphore_GetCount(arg0);
        break;
    default:
        ret = -ENOSYS;
    }

    // return
    sp[0] = ret;

    __enable_irq();
}

// PendSV: 현재 태스크 SP 저장 -> 스택 오버플로 검사 -> 스케줄링 -> 다음 태스크 SP 반환
static __attribute__((used))
uint32_t sPendSV_SwitchContext(uint32_t psp)
{
    KernelTcb_t* tcb = TaskManager_Get_Current_Task();
    tcb->sp = (uint32_t*)psp;

    // 스택 바닥의 canary가 깨졌으면 오버플로
    if (*(uint32_t const*)tcb->stack_base != (uint32_t)STACK_CANARY_VALUE)
    {
        sStackOverflow_Dump((uint32_t*)psp);
        return 0; // unreachable
    }

    TaskManager_Scheduler();
    tcb = TaskManager_Get_Current_Task();
    return (uint32_t)tcb->sp;
}

// HardFault, MemManage, BusFault, UsageFault Handler에서 호출
// sp: 하드웨어가 예외 진입 시 쌓은 스택프레임; sp[0..7] = r0-r3, r12, lr, pc, psr
static __attribute((used))
void sFaultDump(uint32_t* sp)
{
    uint32_t const cfsr = SCB->CFSR;
    uint32_t const hfsr = SCB->HFSR;
    uint32_t const dfsr = SCB->DFSR;
    uint32_t const mmfar = SCB->MMFAR;
    uint32_t const bfar = SCB->BFAR;
    uint32_t const afsr = SCB->AFSR;
    uint32_t const shcsr = SCB->SHCSR;
    printf("\r\n[[[[[[[[[[[[[[ KERNEL PANIC ]]]]]]]]]]]]]]\r\n");
    printf("\r\n==============  FAULT DUMP  ==============\r\n");

    printf("\r\nSystem Handler Control & State (SHCSR)=0x%08lX\r\n", (unsigned long)shcsr);

    printf("\r\nHardFault Status (HFSR)=0x%08lX\r\n", (unsigned long)hfsr);
    prflag(hfsr & SCB_HFSR_VECTTBL_Msk, "VECTTBL: BusFault on vector fetch");
    prflag(hfsr & SCB_HFSR_FORCED_Msk, "FORCED : Escalated fault (see CFSR)");
#if defined(SCB_HFSR_DEBUGEVT_Msk)
    prflag(hfsr & SCB_HFSR_DEBUGEVT_Msk, "DEBUGEVT: Debug event");
#endif

    printf("\r\nConfigurable Fault Status (CFSR)=0x%08lX\r\n", (unsigned long)cfsr);

    // MemManage Fault Status (MMFSR: [7:0])
    uint32_t const mmfsr = (cfsr & 0x000000FFU);
    if (mmfsr)
    {
        printf("  MemManage Fault (MMFSR)=0x%02lX\r\n", (unsigned long)mmfsr);
        prflag(mmfsr & (1u << 0), "IACCVIOL: Instruction access violation");
        prflag(mmfsr & (1u << 1), "DACCVIOL: Data access violation");
        prflag(mmfsr & (1u << 3), "MUNSTKERR: Unstacking error");
        prflag(mmfsr & (1u << 4), "MSTKERR: Stacking error");
        prflag(mmfsr & (1u << 5), "MLSPERR: Lazy FP stacking error (M4F/M7F)");
        if (mmfsr & (1u << 7))
        {
            printf("  -> MMFAR=0x%08lX (valid)\r\n", (unsigned long)mmfar);
        }
    }

    // BusFault Status (BFSR: [15:8])
    uint32_t const bfsr = (cfsr >> 8U) & 0xFFU;
    if (bfsr)
    {
        printf("  BusFault (BFSR)=0x%02lX\r\n", (unsigned long)bfsr);
        prflag(bfsr & (1U << 0U), "IBUSERR: Instruction bus error");
        prflag(bfsr & (1U << 1U), "PRECISERR: Precise data bus error");
        prflag(bfsr & (1U << 2U), "IMPRECISERR: Imprecise data bus error");
        prflag(bfsr & (1U << 3U), "UNSTKERR: Unstacking error");
        prflag(bfsr & (1U << 4U), "STKERR: Stacking error");
        prflag(bfsr & (1U << 5U), "LSPERR: Lazy FP stacking error (M4F/M7F)");
        if (bfsr & (1U << 7U))
        {
            printf("  -> BFAR=0x%08lX (valid)\r\n", (unsigned long)bfar);
        }
    }

    // UsageFault Status (UFSR: [31:16])
    uint32_t const ufsr = (cfsr >> 16U);
    if (ufsr)
    {
        printf("  UsageFault (UFSR)=0x%04lX\r\n", (unsigned long)ufsr);
        prflag(ufsr & (1u << 0), "UNDEFINSTR: Undefined instruction");
        prflag(ufsr & (1u << 1), "INVSTATE: Invalid state");
        prflag(ufsr & (1u << 2), "INVPC: Invalid PC load");
        prflag(ufsr & (1u << 3), "NOCP: No coprocessor");
        prflag(ufsr & (1u << 8), "UNALIGNED: Unaligned access trap");
        prflag(ufsr & (1u << 9), "DIVBYZERO: Divide by zero");
    }

    printf("\r\nDebug Fault Status (DFSR)=0x%08lX\r\n", (unsigned long)dfsr);
    printf("Auxiliary Fault Status (AFSR)=0x%08lX\r\n", (unsigned long)afsr);

    // 현재 SP, MSP/PSP, CONTROL
    bool const sp_sel = __get_CONTROL() & 2;
    printf("\r\nSP sel: %s\r\n", (sp_sel ? "PSP" : "MSP"));
    printf("  MSP=0x%08lX  PSP=0x%08lX  CONTROL=0x%08lX\r\n",
           (unsigned long)__get_MSP(), (unsigned long)__get_PSP(), (unsigned long)__get_CONTROL());

    if (sp_sel)
    {
        int32_t const task_id = (int32_t)TaskManager_Get_Current_Task()->id;
        printf("Task ID: %ld\r\n", task_id);

        printf("\r\nTask Stack Info:\r\n");
        int32_t const usage = TASK_STACK_TOP - task_id * TASK_STACK_SIZE - (int32_t)sp;
        printf("    SP: 0x%08lX    %ld/%d bytes used.\r\n", (unsigned long)sp, usage, TASK_STACK_SIZE);
    }

    printf("\r\nStacked Registers at time of fault:\r\n");
    printf("  R0:  0x%08lX    R1:  0x%08lX    R2:  0x%08lX\r\n", sp[0], sp[1], sp[2]);
    printf("  R3:  0x%08lX    R12: 0x%08lX    LR:  0x%08lX\r\n", sp[3], sp[4], sp[5]);
    printf("  PC:  0x%08lX    PSR: 0x%08lX\r\n", sp[6], sp[7]);

    printf("\r\n============= END FAULT DUMP =============\r\n");

    while (1)
    {
    }
}

// PendSV에서 태스크 스택 canary가 깨진 것을 발견했을 때 호출
// sp: Handler가 r4-r11 백업을 마친 태스크 SP; sp[0..7] = r4-r11; sp[8..15] = 예외 스택프레임(r0-r3, r12, lr, pc, psr)
static __attribute__((used))
void sStackOverflow_Dump(uint32_t* sp)
{
    printf("\r\n[[[[[[[[[[[[[[ KERNEL PANIC ]]]]]]]]]]]]]]\r\n");
    printf("\r\n=========  TASK STACK OVERFLOW  ==========\r\n");

    int32_t const task_id = (int32_t)TaskManager_Get_Current_Task()->id;
    printf("Task ID: %ld\r\n", task_id);

    printf("\r\nTask Stack Info:\r\n");
    int32_t const usage = TASK_STACK_TOP - task_id * TASK_STACK_SIZE - (int32_t)sp;
    printf("  PSP: 0x%08lX    %ld/%d bytes used.\r\n", (unsigned long)sp, usage, TASK_STACK_SIZE);

    printf("\r\nStacked Registers at time of fault:\r\n");
    printf("  R0:  0x%08lX    R1:  0x%08lX    R2:  0x%08lX\r\n", sp[8], sp[9], sp[10]);
    printf("  R3:  0x%08lX    R4:  0x%08lX    R5:  0x%08lX\r\n", sp[11], sp[0], sp[1]);
    printf("  R6:  0x%08lX    R7:  0x%08lX    R8:  0x%08lX\r\n", sp[2], sp[3], sp[4]);
    printf("  R9:  0x%08lX    R10: 0x%08lX    R11: 0x%08lX\r\n", sp[5], sp[6], sp[7]);
    printf("  R12: 0x%08lX    LR:  0x%08lX    PC:  0x%08lX\r\n", sp[12], sp[13], sp[14]);
    printf("  PSR: 0x%08lX\r\n", sp[15]);

    printf("\r\n============= END FAULT DUMP =============\r\n");

    while (1)
    {
    }
}

static __attribute((used))
void prflag(uint32_t cond, const char* name)
{
    if (cond) printf("  - %s\r\n", name);
}
/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern UART_HandleTypeDef huart2;
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M3 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
    /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

    /* USER CODE END NonMaskableInt_IRQn 0 */
    /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
    while (1)
    {
    }
    /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
__attribute((naked))
void HardFault_Handler(void)
{
    /* USER CODE BEGIN HardFault_IRQn 0 */
    // 예외 스택프레임(msp/psp)을 r0에 담아 덤프 함수로 tail branch
    __asm volatile(
        "TST   lr, #4              \n" // EXC_RETURN의 2번 비트로 PSP/MSP 선택
        "ITE   eq                  \n"
        "MRSEQ r0, msp             \n"
        "MRSNE r0, psp             \n"
        "B     sFaultDump          \n"
        // sFaultDump에서 복귀하지 않음
    );
    /* USER CODE END HardFault_IRQn 0 */
}

/**
  * @brief This function handles Memory management fault.
  */
__attribute((naked))
void MemManage_Handler(void)
{
    /* USER CODE BEGIN MemoryManagement_IRQn 0 */
    // 예외 스택프레임(msp/psp)을 r0에 담아 덤프 함수로 tail branch
    __asm volatile(
        "TST   lr, #4              \n" // EXC_RETURN의 2번 비트로 PSP/MSP 선택
        "ITE   eq                  \n"
        "MRSEQ r0, msp             \n"
        "MRSNE r0, psp             \n"
        "B     sFaultDump          \n"
        // sFaultDump에서 복귀하지 않음
    );
    /* USER CODE END MemoryManagement_IRQn 0 */
}

/**
  * @brief This function handles Prefetch fault, memory access fault.
  */
__attribute((naked))
void BusFault_Handler(void)
{
    /* USER CODE BEGIN BusFault_IRQn 0 */
    // 예외 스택프레임(msp/psp)을 r0에 담아 덤프 함수로 tail branch
    __asm volatile(
        "TST   lr, #4              \n" // EXC_RETURN의 2번 비트로 PSP/MSP 선택
        "ITE   eq                  \n"
        "MRSEQ r0, msp             \n"
        "MRSNE r0, psp             \n"
        "B     sFaultDump          \n"
        // sFaultDump에서 복귀하지 않음
    );
    /* USER CODE END BusFault_IRQn 0 */
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
__attribute((naked))
void UsageFault_Handler(void)
{
    /* USER CODE BEGIN UsageFault_IRQn 0 */
    // 예외 스택프레임(msp/psp)을 r0에 담아 덤프 함수로 tail branch
    __asm volatile(
        "TST   lr, #4              \n" // EXC_RETURN의 2번 비트로 PSP/MSP 선택
        "ITE   eq                  \n"
        "MRSEQ r0, msp             \n"
        "MRSNE r0, psp             \n"
        "B     sFaultDump          \n"
        // sFaultDump에서 복귀하지 않음
    );
    /* USER CODE END UsageFault_IRQn 0 */
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
__attribute((naked))
void SVC_Handler(void)
{
    __asm volatile(
        "CPSID i                        \n"
        // SVC를 호출한 스택프레임 선택
        "TST   lr, #4                   \n" // EXC_RETURN의 2번 비트로 PSP/MSP 선택
        "ITE   eq                       \n"
        "MRSEQ r0, msp                  \n"
        "MRSNE r0, psp                  \n"
        // SVC 번호 추출
        "LDR   r1, [r0, #24]            \n" // r1 <- stacked PC (SVC 이후 명령)
        "SUBS  r1, #2                   \n" // r1 = SVC 명령 위치 (PC - 2)
        "LDRB  r2, [r1]                 \n" // r2 = SVC #번호 (SVC 명령의 LSB)
        // SVC 번호에 따른 분기
        "CMP   r2, #0                   \n" // SVC#0 = 태스크 시작
        "BEQ   1f                       \n"
        "CMP   r2, #1                   \n" // SVC#1 = System Call 처리
        "BEQ   sSVC_SystemCall          \n" // r0 = 스택프레임, lr = EXC_RETURN 그대로 tail branch
        "CPSIE i                        \n"
        "BX    lr                       \n" // (그 외 번호: 그냥 예외 복귀)
        // SVC#0: 첫 태스크 컨텍스트 복구 / Thread Mode PSP 전환
        "1:                             \n"
        "BL    sSVC_GetStartTaskSp      \n" // r0 = 첫 태스크의 저장된 SP (lr은 아래에서 덮어쓰므로 보존 불필요)
        "LDMIA r0!, {r4-r11}            \n" // 태스크 스택 프레임 복구
        "MSR   psp, r0                  \n" // PSP 복구
        // Control 레지스터를 2(PSP사용, Privileged)로 변경
        "MOVS  r0, #2                   \n"
        "MSR   control, r0              \n"
        "ISB                            \n" // Instruction Sync. Barrier
        // EXC_RETURN 값으로 복귀(PSP활성, Thread Mode 복귀)
        "MOV   lr, #0xFFFFFFFD          \n"
        "CPSIE i                        \n"
        "BX    lr                       \n"
    );
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
    /* USER CODE BEGIN DebugMonitor_IRQn 0 */

    /* USER CODE END DebugMonitor_IRQn 0 */
    /* USER CODE BEGIN DebugMonitor_IRQn 1 */

    /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
__attribute((naked)) void PendSV_Handler(void)
{
    /* USER CODE BEGIN PendSV_IRQn 0 */
    __asm volatile(
        "CPSID i                          \n"
        // 현재 태스크 컨텍스트 백업
        "MRS   r0, psp                    \n"
        "STMDB r0!, {r4-r11}              \n" // r4-r11 백업; r0 = 저장할 태스크 SP
        "PUSH  {r4, lr}                   \n" // EXC_RETURN 보존 (r4는 8byte padding용)
        "BL    sPendSV_SwitchContext      \n" // SP 저장, 오버플로 검사, 스케줄링; r0 = 다음 태스크 SP
        "POP   {r4, lr}                   \n"
        // 다음 태스크 컨텍스트 복원
        "LDMIA r0!, {r4-r11}              \n"
        "MSR   psp, r0                    \n"
        "CPSIE i                          \n"
        "BX    lr                         \n"
    );
    /* USER CODE END PendSV_IRQn 0 */
    /* USER CODE BEGIN PendSV_IRQn 1 */

    /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
    __disable_irq();
    if (Kernel_Is_Running())
    {
        HAL_IncTick();
        TaskManager_SysTick_Callback();
    }
    __enable_irq();

}

/******************************************************************************/
/* STM32F1xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f1xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles USART2 global interrupt.
  */
void USART2_IRQHandler(void)
{
    /* USER CODE BEGIN USART2_IRQn 0 */

    /* USER CODE END USART2_IRQn 0 */
    HAL_UART_IRQHandler(&huart2);
    /* USER CODE BEGIN USART2_IRQn 1 */

    /* USER CODE END USART2_IRQn 1 */
}

/**
  * @brief This function handles EXTI line[15:10] interrupts.
  */
void EXTI15_10_IRQHandler(void)
{
    /* USER CODE BEGIN EXTI15_10_IRQn 0 */

    /* USER CODE END EXTI15_10_IRQn 0 */
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
    /* USER CODE BEGIN EXTI15_10_IRQn 1 */

    /* USER CODE END EXTI15_10_IRQn 1 */
}

/* USER CODE BEGIN 1 */
/* USER CODE END 1 */

#ifdef __cplusplus
}
#endif
