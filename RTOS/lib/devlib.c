#include "devlib.h"

#include "BspSysTick.h"

void delay(uint32_t ms) {
	BSP_Delay(ms);
}

