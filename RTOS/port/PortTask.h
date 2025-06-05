/*
 * PortTask.h
 *
 *  Created on: May 26, 2025
 *      Author: moon
 */

#ifndef PORT_PORTTASK_H_
#define PORT_PORTTASK_H_

#include <stdint.h>

#include "cortex-m3/context.h"

void Port_Task_Create(TaskStackFrame_t* task_frame, uint32_t pc);
void Port_Task_Start(void);

#endif /* PORT_PORTTASK_H_ */
