#ifndef LIB_DEVLIB_H_
#define LIB_DEVLIB_H_

#include "stdint.h"

#define SET_BIT(REG, BIT)   ((REG) |= (BIT))
#define CLR_BIT(REG, BIT) ((REG) &= ~(BIT))

void delay(uint32_t ms);

//void memclr(void* dist, uint32_t count);

#endif // LIB_DEVLIB_H_
