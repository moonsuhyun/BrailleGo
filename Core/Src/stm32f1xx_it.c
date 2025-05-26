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
#include "main.h"
#include "stm32f1xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "switch.h"
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
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

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
    asm volatile (
        // 스택프레임 선택
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
    	"CMP   r2, #1			   \n"  // SVC#1 = 컨택스트 스위칭을 위해 PendSV 활성
    	"BEQ   svc_trigger_pendsv  \n"
//		"BEQ   Port_trigger_pendsv \n"
        "BX    lr                  \n"	// (그 외 번호: 그냥 예외 복귀)

		// 태스크 시작 컨텍스트 복구/Thread Mode PSP 전환
    	"svc_task_start:           \n"
		"LDR   r0, =Next_tcb       \n"
		"LDR   r0, [r0]        	   \n"
		"LDR   r0, [r0]            \n"  // r0 = Next_tcb->sp
		"LDMIA r0!, {r4-r11}       \n"  // 태스크 스택 프레임 복구
		"MSR   psp, r0             \n"  // PSP 복구
		// Control 레지스터를 2(PSP사용, Privileged)로 변경
		"MOVS  r0, #2              \n"
		"MSR   control, r0         \n"
		"ISB                       \n"  // Instruction Sync. Barrier
		// EXC_RETURN 값으로 복귀(PSP활성, Thread Mode 복귀)
		"MOV   lr, #0xFFFFFFFD     \n"
		"BX    lr                  \n"
		"svc_trigger_pendsv:        \n"
		"LDR r0, =0xE000ED04\n"
		"LDR r1, [r0]\n"
		"ORR r1, r1, #0x10000000\n"
		"STR r1, [r0]\n"
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
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */
	asm volatile (
		// 현재 태스크 컨텍스트 백업
		"MRS   r0, psp             \n"
		"STMDB r0!, {r4-r11}       \n"  // 현재 태스크 스택 프레임 백업
		"LDR   r1, =Current_tcb    \n"
		"LDR   r1, [r1]            \n"  // r1 = Current_tcp->sp
		"STR   r0, [r1]            \n"  // PSP 백업
		// 다음 태스크 컨택스트 복원
		"LDR   r1, =Next_tcb       \n"
		"LDR   r1, [r1]            \n"
		"LDR   r0, [r1]            \n"  // r0 = Next_tcb->sp
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
  if (TIMESLICE_SCHEDULING) Kernel_systick_callback();
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
  HAL_GPIO_EXTI_IRQHandler(B1_Pin);
  /* USER CODE BEGIN EXTI15_10_IRQn 1 */

  /* USER CODE END EXTI15_10_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
