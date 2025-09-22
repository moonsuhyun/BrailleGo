/*
 * BspSysTick.c
 *
 *  Created on: May 26, 2025
 *      Author: moon
 */

#include "BspSysTick.h"

#include "stm32f1xx_hal.h"

void BSP_Delay(uint32_t ms) {
	HAL_Delay(ms);
}

uint32_t BSP_Get_Tick(void) {
	return HAL_GetTick();
}
