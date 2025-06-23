/*
 * usertask.c
 *
 *  Created on: May 21, 2025
 *      Author: moon
 */
#include <stdio.h>
#include "devio.h"
#include "devlib.h"
#include "kernel.h"
#include "stdlib.h"

//__attribute ((used)) volatile void Task1(void) {
//    while (1) {
//        printf("[TICK=%lu] Task1\r\n", Kernel_Get_SysTick());
//        Kernel_Delay(1000); // 100 ticks
//    }
//}
//
//__attribute ((used)) volatile void Task2(void) {
//    while (1) {
//        printf("[TICK=%lu] Task2\r\n", Kernel_Get_SysTick());
//        Kernel_Delay(2000); // 150 ticks
//    }
//}
//
//__attribute ((used)) volatile void Task3(void) {
//    while (1) {
//        printf("[TICK=%lu] Task3\r\n", Kernel_Get_SysTick());
//        Kernel_Delay(3000); // 150 ticks
//    }
//}

void TaskA(void) {
    while (1) {
        printf("[Tick %u] TaskA run\r\n", Kernel_Get_SysTick());
        Kernel_Delay(1000);       // BLOCKED 80tick
        printf("[Tick %u] TaskA after delay\r\n", Kernel_Get_SysTick());
        Kernel_Yield(); // 바로 Ready queue tail에 삽입
    }
}
void TaskB(void) {
    while (1) {
        printf("[Tick %u] TaskB run\r\n", Kernel_Get_SysTick());
        Kernel_Delay(2000);
        printf("[Tick %u] TaskB after delay\r\n", Kernel_Get_SysTick());
        Kernel_Yield();
    }
}

void TaskC(void) {
	while (1) {
		printf("[Tick %u] TaskC RUN\r\n", Kernel_Get_SysTick());
		Kernel_Delay(2000);
		printf("[Tick %u] TaskC UNBLOCK\r\n", Kernel_Get_SysTick());
	}
}
void TaskD(void) {
	while (1) {
		printf("[Tick %u] TaskD RUN\r\n", Kernel_Get_SysTick());
		Kernel_Delay(2000);
		printf("[Tick %u] TaskD UNBLOCK\r\n", Kernel_Get_SysTick());
	}
}
void TaskE(void) {
	while (1) {
		printf("[Tick %u] TaskE RUN\r\n", Kernel_Get_SysTick());
		Kernel_Delay(1000);
		printf("[Tick %u] TaskE UNBLOCK\r\n", Kernel_Get_SysTick());
	}
}
void TaskF(void) {
	while (1) {
		printf("[Tick %u] TaskF RUN\r\n", Kernel_Get_SysTick());
		Kernel_Delay(0);
		printf("[Tick %u] TaskF UNBLOCK\r\n", Kernel_Get_SysTick());
	}
}


