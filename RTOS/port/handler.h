/*
 * handler.h
 *
 *  Created on: May 26, 2025
 *      Author: moon
 */

#ifndef PORT_HANDLER_H_
#define PORT_HANDLER_H_

#include "task.h"

__attribute ((naked)) void Port_svc0_handler(void); // Start first user task
__attribute ((naked)) void Port_svc1_handler(void); // Context switching

#endif /* PORT_HANDLER_H_ */
