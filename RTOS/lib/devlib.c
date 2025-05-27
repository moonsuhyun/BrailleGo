#include "devlib.h"

#include "BspSysTick.h"

void delay(uint32_t ms) {
	Bsp_delay(ms);
}



//void memclr(void* dist, uint32_t count) {
//    uint8_t* d = (uint8_t*)dist;
//    while(count--) *d++ = 0;
//}
