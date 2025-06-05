/*
 * BspUart.c
 *
 *  Created on: May 26, 2025
 *      Author: moon
 */

#include "BspUart.h"

#include "stm32f1xx_hal.h"

extern UART_HandleTypeDef huart2;

void BSP_UART_Transmit(uint8_t* data, uint32_t len) {
	HAL_UART_Transmit(&huart2, data, len, HAL_MAX_DELAY);
}

//void Bsp_uart_put_char(const uint8_t ch) {
//	HAL_UART_Transmit(&huart2, &ch, 1, HAL_MAX_DELAY);
//}
