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

void Task0(void) {
	int a;
	while (1) {
		debug_printf("User task 0: &a = %u", &a);
		Kernel_yield();
		delay(500);
		debug_printf("User task 0: continue");
	}
}
void Task1(void) {
	int b;
	while (1) {
		debug_printf("User task 1: &b = %u", &b);
		Kernel_yield();
		delay(500);
		debug_printf("User task 1: continue");
	}
}
