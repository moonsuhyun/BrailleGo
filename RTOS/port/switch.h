/*
 * switch.h
 *
 *  Created on: May 26, 2025
 *      Author: moon
 */

#ifndef PORT_SWITCH_H_
#define PORT_SWITCH_H_

#include "stdint.h"
#include "handler.h"

//#if defined(BOARD_STM32F103)
//	#include "stm32f103/MemoryMap.h"
//#endif

#include "cortex-m3/context.h"
#define ENTER_SWITCH_CRITICAL   __set_BASEPRI(0x10)
#define EXIT_SWITCH_CRITICAL 	__set_BASEPRI(0x0)

void Port_Task_Create(TaskStackFrame_t* task_frame, uint32_t pc);
void Port_task_start(void);
void Port_trigger_pendsv(void);

void Port_switch_enter_critical(void);
void Port_switch_exit_critical(void);

//void Port_trigger_pendsv(void);
//void Port_trigger_svc(uint32_ svc_no);
//void Port_systick_callback(void);

#endif /* PORT_SWITCH_H_ */
