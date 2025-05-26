/*
 * handler.c
 *
 *  Created on: May 26, 2025
 *      Author: moon
 */

#include "handler.h"

extern KernelTcb_t* Current_tcb;
extern KernelTcb_t* Next_tcb;

__attribute ((naked)) void Port_svc0_handler(void) {
	asm volatile (
			"LDR   r0, =Next_tcb   \n"
			"LDR   r0, [r0]        \n"
			"LDR   r0, [r0]        \n"
			"LDMIA r0!, {r4-r11}   \n"  // Task Stack Frame
			"MSR   psp, r0         \n"  // PSP 복구

			"MOVS  r0, #2          \n"
			"MSR   control, r0     \n"  // PSP mode

			"ISB                   \n"  // Instruction Sync. Barrier

			"MOV   lr, #0xFFFFFFFD \n"  // EXC_RETURN for thread mode
			"BX    lr              \n"
			);
}

__attribute ((naked)) void Port_svc1_handler(void) {
	asm volatile (
			"MRS   r0, psp         \n"
			"STMDB r0!, {r4-r11}   \n"
			"LDR   r1, =Current_tcb\n"
			"LDR   r1, [r1]        \n"
			"STR   r0, [r1]        \n"

			"LDR   r1, =Next_tcb   \n"
			"LDR   r1, [r1]        \n"
			"LDR   r0, [r1]        \n"
			"LDMIA r0!, {r4-r11}   \n"
			"MSR   psp, r0         \n"
			"MOV   lr, #0xFFFFFFFD \n"  // EXC_RETURN for thread mode
			"BX    lr              \n"
			);
}
