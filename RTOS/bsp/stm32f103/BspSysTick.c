/*
 * BspSysTick.c
 *
 *  Created on: May 26, 2025
 *      Author: moon
 */


#include "BspSysTick.h"
#include "stm32f1xx_hal.h"

void Bsp_delay(uint32_t ms) {
	HAL_Delay(ms);
}
