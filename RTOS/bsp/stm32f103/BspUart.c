/*
 * BspUart.c
 *
 *  Created on: May 26, 2025
 *      Author: moon
 */

#include "stm32f1xx_hal.h"
#include "BspUart.h"

extern UART_HandleTypeDef huart2;

void Bsp_uart_put_char(const uint8_t ch) {
	HAL_UART_Transmit(&huart2, &ch, 1, HAL_MAX_DELAY);
}
