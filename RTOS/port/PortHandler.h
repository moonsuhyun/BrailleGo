/*
 * PortHandler.h
 *
 *  Created on: May 26, 2025
 *      Author: moon
 */

#ifndef PORT_PORTHANDLER_H_
#define PORT_PORTHANDLER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void Port_Handler_HardFault(uint32_t* sp);
void Port_Handler_SVC(void);    // Start first user task
void Port_Handler_PendSV(void); // Context switching
void Port_Handler_SysTick(void);

#ifdef __cplusplus
}
#endif

#endif /* PORT_PORTHANDLER_H_ */
