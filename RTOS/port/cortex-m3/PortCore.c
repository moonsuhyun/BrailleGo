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


inline void Port_Core_Interrupt_Init(void) {
	SCB->CCR |= SCB_CCR_STKALIGN_Msk;

	SCB->SHCSR |= SCB_SHCSR_USGFAULTENA_Msk |
		SCB_SHCSR_BUSFAULTENA_Msk |
		SCB_SHCSR_MEMFAULTENA_Msk;

	HAL_NVIC_SetPriority(PendSV_IRQn,  15, 0);
	HAL_NVIC_SetPriority(SysTick_IRQn, 14, 0);
	HAL_NVIC_SetPriority(SVCall_IRQn,  13, 0);
}

inline void Port_Core_Enable_IRQ(void) {
	__enable_irq();
}

inline void Port_Core_Disable_IRQ(void) {
	__disable_irq();
}

inline void Port_Core_Enable_PendSV(void) {
	__set_BASEPRI(0x0);
}

inline void Port_Core_Disable_PendSV(void) {
	__set_BASEPRI(0x10);
}

inline void Port_Core_Wait_For_Interrupt(void) {
	__WFI();
}

inline void Port_Core_Trigger_PendSV(void) {
	// asm volatile ("SVC 1");
	SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
}

int Port_Core_SVC_Call(int sysno, void* a0, void* a1, void* a2)
{
	register int   r0 __asm("r0") = sysno;
	register void *r1 __asm("r1") = a0;
	register void *r2 __asm("r2") = a1;
	register void *r3 __asm("r3") = a2;

	__asm volatile(
		"svc 1"
		: "+r"(r0)
		: "r"(r1), "r"(r2), "r"(r3)
		: "memory"
	);

	return r0;
}
