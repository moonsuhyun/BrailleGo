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
	int a;
	while (1) {
		debug_printf("User task 0: &a = %x", &a);
		Kernel_yield();
		delay(500);
		debug_printf("User task 0: continue");
	}
}
__attribute ((used)) volatile void Task1(void) {
	int b;
	while (1) {
		debug_printf("User task 1: &b = %x", &b);
		Kernel_yield();
		delay(500);
		debug_printf("User task 1: continue");
	}
}
