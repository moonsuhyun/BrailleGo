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

__attribute ((used)) volatile void Task1(void) {
    while (1) {
        printf("[TICK=%lu] Task1\r\n", Kernel_Get_SysTick());
        Kernel_Delay(1000); // 100 ticks
    }
}

__attribute ((used)) volatile void Task2(void) {
    while (1) {
        printf("[TICK=%lu] Task2\r\n", Kernel_Get_SysTick());
        Kernel_Delay(2000); // 150 ticks
    }
}

__attribute ((used)) volatile void Task3(void) {
    while (1) {
        printf("[TICK=%lu] Task3\r\n", Kernel_Get_SysTick());
        Kernel_Delay(3000); // 150 ticks
    }
}

