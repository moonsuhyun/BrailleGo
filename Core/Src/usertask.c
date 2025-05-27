/*
 * usertask.c
 *
 *  Created on: May 21, 2025
 *      Author: moon
 */
#include "devio.h"
#include "devlib.h"
#include "kernel.h"
#include "stdlib.h"

__attribute ((used)) volatile void Task0(void) {
	while (1) {
		Kernel_yield();
	}
}
__attribute ((used)) volatile void Task1(void) {
	while (1) {
		Kernel_yield();
	}
}
