//
// Created by ��������� on 25. 9. 17..
//

#ifndef BSP_BSPHWINIT_H_
#define BSP_BSPHWINIT_H_

#include "stm32f1xx_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

void Error_Handler(void);

void SystemClock_Config(void);
void MX_GPIO_Init(void);
void MX_USART2_UART_Init(void);

#ifdef __cplusplus
    }
#endif

#endif /* BSP_BSPHWINIT_H_ */