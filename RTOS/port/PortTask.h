/*
 * PortTask.h
 *
 *  Created on: May 26, 2025
 *      Author: moon
 */

#ifndef PORT_PORTTASK_H_
#define PORT_PORTTASK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "cortex-m3/context.h"

void Port_Task_Create(TaskStackFrame_t* task_frame, uint32_t pc);
void Port_Task_Start(void);


#ifdef __cplusplus
}
#endif


#endif /* PORT_PORTTASK_H_ */
