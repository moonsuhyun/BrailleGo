/*
 * PortTask.c
 *
 *  Created on: May 26, 2025
 *      Author: moon
 */

#include "PortTask.h"

void Port_Task_Create(TaskStackFrame_t* task_frame, uint32_t pc) {
    task_frame->exc_frame.xpsr = 0x01000000UL;
    task_frame->exc_frame.pc = pc;
    task_frame->exc_frame.lr = 0xFFFFFFFDUL; // EXC_RETURN for thread mode
}

void Port_Task_Start(void) {
	asm volatile ("SVC 0");
}


//__attribute ((naked)) void Port_Core_Trigger_PendSV(void) {
//    SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
//    __asm volatile ("BX lr");
//}

