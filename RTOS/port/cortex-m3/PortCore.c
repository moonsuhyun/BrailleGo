/*
 * PortCore.c
 *
 *  Created on: May 28, 2025
 *      Author: moon
 */


#include "PortCore.h"

#include <stdint.h>
#include <cmsis_gcc.h>


void Port_Core_Interrupt_Init(void) {

}

void Port_Core_Enable_IRQ(void) {

}

void Port_Core_Disable_IRQ(void) {

}

void Port_Core_Enable_PendSV(void) {
	__set_BASEPRI(0x0);
}

void Port_Core_Disable_PendSV(void) {
	__set_BASEPRI(0x10);
}

void Port_Core_Wait_For_Interrupt(void) {
	__WFI();
}

void Port_Core_Trigger_PendSV(void) {
	asm volatile ("SVC 1");
}
