/*
 * PortTask.c
 *
 *  Created on: May 26, 2025
 *      Author: moon
 */

#include "PortTask.h"

#include "cmsis_gcc.h"
#include "Types.h"
#include "TaskManager.h"

void Port_Task_Create(TaskStackFrame_t* task_frame, uint32_t wrapper, uint32_t task_func, void* arg) {
    task_frame->exc_frame.xpsr = 0x01000000UL;
    task_frame->exc_frame.pc = wrapper;
    task_frame->exc_frame.lr = 0xFFFFFFFDUL; // EXC_RETURN for thread mode
    task_frame->exc_frame.r0_r3[0] = task_func;
    task_frame->exc_frame.r0_r3[1] = (uint32_t) arg;
}

// void Port_Task_Start(void) {
// 	asm volatile ("SVC 0");
// }

__attribute((noreturn))
inline void Port_Task_Start(void)
{
    __asm volatile ("SVC 0");
    __builtin_unreachable();
}


//__attribute ((naked)) void Port_Core_Trigger_PendSV(void) {
//    SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
//    __asm volatile ("BX lr");
//}

