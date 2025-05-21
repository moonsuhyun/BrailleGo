#include <devlib.h>
#include "stdint.h"
#include "stm32f1xx_hal.h"


void delay(uint32_t ms) {
	HAL_Delay(ms);
}

//void memclr(void* dist, uint32_t count) {
//    uint8_t* d = (uint8_t*)dist;
//    while(count--) *d++ = 0;
//}
