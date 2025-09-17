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
#include "PortHandler.h"
#include "stm32f1xx_hal.h"
#include <Kernel.h>
#include <stdio.h>
#include <TaskManager.h>
#include "types.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define TIMESLICE_SCHEDULING 0
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
static KernelTcb_t* current_tcb;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
void HardFault_Handler(uint32_t* sp)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */
	__asm volatile (
            "tst   lr, #4        \n"
            "ite   eq            \n"
            "mrseq r0, msp       \n"
            "mrsne r0, psp       \n"

    );
	uint32_t pc = sp[6];
	printf("pc=%x\n", pc);
  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Prefetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
__attribute((naked)) void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */
	asm volatile ("PUSH  {r7, lr}\n");
	current_tcb = (KernelTcb_t*) Kernel_Task_Get_Current_Task();
	asm volatile ("POP   {r7, lr}\n");
	asm volatile (
		// SVC를 호출한 스택프레임 선택
		"TST   lr, #4              \n"	// EXC_RETURN의 2번 비트로 PSP/MSP 선택
		"ITE   eq                  \n"
		"MRSEQ r0, msp             \n"
		"MRSNE r0, psp             \n"
		// SVC 번호 추출
		"LDR   r1, [r0, #24]       \n"	// r1 <- stacked PC (SVC 이후 명령)
		"SUBS  r1, #2              \n"	// r1 = SVC 명령 위치 (PC - 2)
		"LDRB  r2, [r1]            \n"	// r2 = SVC #번호 (SVC 명령의 LSB)
		// SVC 번호에 따른 분기
		"CMP   r2, #0              \n"  // SVC#0 = 태스크 시작
		"BEQ   svc_task_start 	   \n"
		"CMP   r2, #1			   \n"  // SVC#1 = 컨택스트 스위칭을 위해 PendSV 발생
		"BEQ   svc_trigger_pendsv  \n"
		"BX    lr                  \n"	// (그 외 번호: 그냥 예외 복귀)

		// 태스크 시작 컨텍스트 복구/Thread Mode PSP 전환
		"svc_task_start:           \n"
		"LDR   r0, =current_tcb   \n"
		"LDR   r0, [r0]        	   \n"
		"LDR   r0, [r0]            \n"  // r0 = gCurrent_tcb->sp
		"LDMIA r0!, {r4-r11}       \n"  // 태스크 스택 프레임 복구
		"MSR   psp, r0             \n"  // PSP 복구
		// Control 레지스터를 2(PSP사용, Privileged)로 변경
		"MOVS  r0, #2              \n"
		"MSR   control, r0         \n"
		"ISB                       \n"  // Instruction Sync. Barrier
		// EXC_RETURN 값으로 복귀(PSP활성, Thread Mode 복귀)
		"MOV   lr, #0xFFFFFFFD     \n"
		"BX    lr                  \n"
		// 컨택스트 스위칭을 처리하기 위해 PendSV 발생
		"svc_trigger_pendsv:       \n"
		"LDR   r0, =0xE000ED04     \n"  // SCB->ICSR 주소
		"LDR   r1, [r0]            \n"
		"ORR   r1, r1, #0x10000000 \n"  // 28번 bit(PENDSVSET) set
		"STR   r1, [r0]            \n"  // ICSR에 저장해서 PendSV 발생
		"BX    lr                  \n"
	);
  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
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
	// uint32_t stack_canary_value = STACK_CANARY_VALUE;
	asm volatile ("PUSH  {r7, lr}\n");
	current_tcb = (KernelTcb_t*) Kernel_Task_Get_Current_Task();
	asm volatile ("POP   {r7, lr}\n");
	asm volatile (
		// 현재 태스크 컨텍스트 백업
		"MRS   r0, psp             \n"
		"STMDB r0!, {r4-r11}       \n"  // 현재 태스크 스택 프레임 백업
		"LDR   r1, =current_tcb    \n"
		"LDR   r1, [r1]            \n"  // r1 = Current_tcp->sp
		"STR   r0, [r1]            \n"  // PSP 백업
	);
	asm volatile ("PUSH  {r7, lr}\n");
	Kernel_Task_Scheduler();
	current_tcb = (KernelTcb_t*) Kernel_Task_Get_Current_Task();
	asm volatile ("POP   {r7, lr}\n");
	asm volatile (
		// 다음 태스크 컨택스트 복원
		"LDR   r1, =current_tcb    \n"
		"LDR   r1, [r1]            \n"
		"LDR   r0, [r1]            \n"  // r0 = Next_tcb->sp
		// "LDR   r1, [r1, #4]        \n"  // r1 = Next_tcb->stack_base
		// "LDR   r2, =stack_canary_value\n"
		// "CMP   r1, r2              \n"
		// "BNE   MemManage_Handler   \n"
		"LDMIA r0!, {r4-r11}       \n"  // 다음 태스크 스택 프레임 복구
		"MSR   psp, r0             \n"  // PSP 복구
		"BX    lr                  \n"
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
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */
	if (Kernel_Is_Running()) {
		Kernel_Task_SysTick_Callback();
	}
  /* USER CODE END SysTick_IRQn 1 */
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


// void Port_Handler_HardFault(uint32_t* sp) {
// 	uint32_t pc = sp[6];
// 	printf("pc=%x", pc);
// 	while (1) {
//
// 	}
// }
//
// __attribute ((naked)) void Port_Handler_SVC(void) {
// 	asm volatile ("PUSH  {r7, lr}\n");
// 	current_tcb = Kernel_Task_Get_Current_Task();
// 	asm volatile ("POP   {r7, lr}\n");
//     asm volatile (
//         // SVC를 호출한 스택프레임 선택
//         "TST   lr, #4              \n"	// EXC_RETURN의 2번 비트로 PSP/MSP 선택
//         "ITE   eq                  \n"
//         "MRSEQ r0, msp             \n"
//         "MRSNE r0, psp             \n"
//         // SVC 번호 추출
//         "LDR   r1, [r0, #24]       \n"	// r1 <- stacked PC (SVC 이후 명령)
//         "SUBS  r1, #2              \n"	// r1 = SVC 명령 위치 (PC - 2)
//         "LDRB  r2, [r1]            \n"	// r2 = SVC #번호 (SVC 명령의 LSB)
//         // SVC 번호에 따른 분기
//         "CMP   r2, #0              \n"  // SVC#0 = 태스크 시작
//         "BEQ   svc_task_start 	   \n"
//     	"CMP   r2, #1			   \n"  // SVC#1 = 컨택스트 스위칭을 위해 PendSV 발생
//     	"BEQ   svc_trigger_pendsv  \n"
//         "BX    lr                  \n"	// (그 외 번호: 그냥 예외 복귀)
//
// 		// 태스크 시작 컨텍스트 복구/Thread Mode PSP 전환
//     	"svc_task_start:           \n"
// 		"LDR   r0, =current_tcb   \n"
// 		"LDR   r0, [r0]        	   \n"
// 		"LDR   r0, [r0]            \n"  // r0 = gCurrent_tcb->sp
// 		"LDMIA r0!, {r4-r11}       \n"  // 태스크 스택 프레임 복구
// 		"MSR   psp, r0             \n"  // PSP 복구
// 		// Control 레지스터를 2(PSP사용, Privileged)로 변경
// 		"MOVS  r0, #2              \n"
// 		"MSR   control, r0         \n"
// 		"ISB                       \n"  // Instruction Sync. Barrier
// 		// EXC_RETURN 값으로 복귀(PSP활성, Thread Mode 복귀)
// 		"MOV   lr, #0xFFFFFFFD     \n"
// 		"BX    lr                  \n"
//     	// 컨택스트 스위칭을 처리하기 위해 PendSV 발생
// 		"svc_trigger_pendsv:       \n"
// 		"LDR   r0, =0xE000ED04     \n"  // SCB->ICSR 주소
// 		"LDR   r1, [r0]            \n"
// 		"ORR   r1, r1, #0x10000000 \n"  // 28번 bit(PENDSVSET) set
// 		"STR   r1, [r0]            \n"  // ICSR에 저장해서 PendSV 발생
// 		"BX    lr                  \n"
//     );
// }
//
// __attribute ((naked)) void Port_Handler_PendSV(void) {
// 	asm volatile ("PUSH  {r7, lr}\n");
// 	current_tcb = Kernel_Task_Get_Current_Task();
// 	asm volatile ("POP   {r7, lr}\n");
// 	asm volatile (
// 		// 현재 태스크 컨텍스트 백업
// 		"MRS   r0, psp             \n"
// 		"STMDB r0!, {r4-r11}       \n"  // 현재 태스크 스택 프레임 백업
// 		"LDR   r1, =current_tcb    \n"
// 		"LDR   r1, [r1]            \n"  // r1 = Current_tcp->sp
// 		"STR   r0, [r1]            \n"  // PSP 백업
// 	);
// 	asm volatile ("PUSH  {r7, lr}\n");
// 	Kernel_Task_Scheduler();
// 	current_tcb = Kernel_Task_Get_Current_Task();
// 	asm volatile ("POP   {r7, lr}\n");
// 	asm volatile (
// 		// 다음 태스크 컨택스트 복원
// 		"LDR   r1, =current_tcb    \n"
// 		"LDR   r1, [r1]            \n"
// 		"LDR   r0, [r1]            \n"  // r0 = Next_tcb->sp
// 		"LDMIA r0!, {r4-r11}       \n"  // 다음 태스크 스택 프레임 복구
// 		"MSR   psp, r0             \n"  // PSP 복구
// 		"BX    lr                  \n"
// 	);
// }
//
// void Port_Handler_SysTick(void) {
// 	if (Kernel_Is_Running()) {
// 		Kernel_Task_SysTick_Callback();
// 	}
// }


#ifdef __cplusplus
}
#endif
