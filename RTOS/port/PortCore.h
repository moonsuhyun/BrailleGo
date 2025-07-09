/*
 * PortCore.h
 *
 *  Created on: May 28, 2025
 *      Author: moon
 */

#ifndef PORT_PORTCORE_H_
#define PORT_PORTCORE_H_

#ifdef __cplusplus
extern "C" {
#endif

void Port_Core_Interrupt_Init(void);

void Port_Core_Enable_IRQ(void);
void Port_Core_Disable_IRQ(void);

void Port_Core_Enable_PendSV(void);
void Port_Core_Disable_PendSV(void);

void Port_Core_Wait_For_Interrupt(void);
void Port_Core_Trigger_PendSV(void);

#ifdef __cplusplus
}
#endif

#endif /* PORT_PORTCORE_H_ */
