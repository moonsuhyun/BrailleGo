/*
 * handler.h
 *
 *  Created on: May 26, 2025
 *      Author: moon
 */

#ifndef PORT_HANDLER_H_
#define PORT_HANDLER_H_

#include "stdio.h"

#include "task.h"
#include "timer.h"
#include "devio.h"
#include "switch.h"

__attribute ((naked)) void Port_SVC_Handler(void);    // Start first user task
__attribute ((naked)) void Port_PendSV_Handler(void); // Context switching
void Port_SysTick_Handler(void);

#endif /* PORT_HANDLER_H_ */
