/*
 * BspUart.h
 *
 *  Created on: May 26, 2025
 *      Author: moon
 */

#ifndef BSP_BSPUART_H_
#define BSP_BSPUART_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void BSP_UART_Transmit(uint8_t* data, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif /* BSP_BSPUART_H_ */
