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

void NMI_Handler(void);
void HardFault_Handler(uint32_t* sp);
void MemManage_Handler(uint32_t* sp);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void SVC_Handler(void);
void DebugMon_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);
void USART2_IRQHandler(void);
void EXTI15_10_IRQHandler(void);

// void Port_Handler_HardFault(uint32_t* sp);
// void Port_Handler_SVC(void);    // Start first user task
// void Port_Handler_PendSV(void); // Context switching
// void Port_Handler_SysTick(void);

#ifdef __cplusplus
}
#endif

#endif /* PORT_PORTHANDLER_H_ */
