/*
 * PortCore.h
 *
 *  Created on: May 28, 2025
 *      Author: moon
 */

#ifndef PORT_PORTCORE_H_
#define PORT_PORTCORE_H_

void Port_Core_Interrupt_Init(void);

void Port_Core_Enable_IRQ(void);
void Port_Core_Disable_IRQ(void);

void Port_Core_Enable_PendSV(void);
void Port_Core_Disable_PendSV(void);

void Port_Core_Wait_For_Interrupt(void);
void Port_Core_Trigger_PendSV(void);

#endif /* PORT_PORTCORE_H_ */
