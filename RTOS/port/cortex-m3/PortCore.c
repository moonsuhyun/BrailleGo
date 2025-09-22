/*
 * PortCore.c
 *
 *  Created on: May 28, 2025
 *      Author: moon
 */


#include "PortCore.h"

#include <stdint.h>
#include <cmsis_gcc.h>
#include <stm32f1xx_hal.h>


void Port_Core_Interrupt_Init(void) {
	SCB->CCR |= SCB_CCR_STKALIGN_Msk;

	SCB->SHCSR |= SCB_SHCSR_USGFAULTENA_Msk |
		SCB_SHCSR_BUSFAULTENA_Msk |
		SCB_SHCSR_MEMFAULTENA_Msk;

	HAL_NVIC_SetPriority(PendSV_IRQn,  15, 0);
	HAL_NVIC_SetPriority(SysTick_IRQn, 14, 0);
	HAL_NVIC_SetPriority(SVCall_IRQn,  13, 0);
}

void Port_Core_Enable_IRQ(void) {
	__enable_irq();
}

void Port_Core_Disable_IRQ(void) {
	__disable_irq();
}

void Port_Core_Enable_PendSV(void) {
	__set_BASEPRI(0x0);
}

void Port_Core_Disable_PendSV(void) {
	__set_BASEPRI(0x10);
}

void Port_Core_Wait_For_Interrupt(void) {
	__WFI();
}

void Port_Core_Trigger_PendSV(void) {
	// asm volatile ("SVC 1");
	SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
}
