/*
 * BspSysTick.h
 *
 *  Created on: May 26, 2025
 *      Author: moon
 */

#ifndef BSP_BSPSYSTICK_H_
#define BSP_BSPSYSTICK_H_

#include <stdint.h>

void BSP_Delay(uint32_t ms);
uint32_t BSP_Get_Tick(void);

#endif /* BSP_BSPSYSTICK_H_ */
