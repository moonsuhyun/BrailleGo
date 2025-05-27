/*
 * BspUart.h
 *
 *  Created on: May 26, 2025
 *      Author: moon
 */

#ifndef BSP_BSPUART_H_
#define BSP_BSPUART_H_

#include "stdint.h"

void Bsp_uart_transmit(uint8_t* data, uint32_t len);

#endif /* BSP_BSPUART_H_ */
