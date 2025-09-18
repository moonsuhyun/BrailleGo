/*
 * BspSysTick.h
 *
 *  Created on: May 26, 2025
 *      Author: moon
 */

#ifndef BSP_BSPSYSTICK_H_
#define BSP_BSPSYSTICK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void BSP_Delay(uint32_t ms);
uint32_t BSP_Get_Tick(void);

#ifdef __cplusplus
}
#endif

#endif /* BSP_BSPSYSTICK_H_ */
