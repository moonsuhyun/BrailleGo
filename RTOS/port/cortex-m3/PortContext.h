/*
 * context.h
 *
 *  Created on: May 26, 2025
 *      Author: moon
 */

#ifndef PORT_CORTEX_M3_CONTEXT_H_
#define PORT_CORTEX_M3_CONTEXT_H_

#include "stdint.h"

// Context switching시 stack에 백업할 레지스터의 구조
typedef struct ExceptionStackFrame {
	uint32_t r0_r3[4];
	uint32_t r12;
	uint32_t lr;
	uint32_t pc;
	uint32_t xpsr;
} ExceptionStackFrame_t;

typedef struct TaskStackFrame {
    uint32_t r4_r11[8];
    ExceptionStackFrame_t exc_frame;
} TaskStackFrame_t;

#endif /* PORT_CORTEX_M3_CONTEXT_H_ */
