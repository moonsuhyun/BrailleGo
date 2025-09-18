/*
 * usertask.c
 *
 *  Created on: May 21, 2025
 *      Author: moon
 */
#include <Kernel.h>
#include <stdio.h>
#include "devlib.h"
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
	uint32_t a=0;
    while (1) {
        printf("[Tick %u] TaskA run &a=%u\r\n", Kernel_Get_SysTick(), &a);
        Kernel_Delay(1000);       // BLOCKED 80tick
        printf("[Tick %u] TaskA after delay\r\n", Kernel_Get_SysTick());
        Kernel_Yield(); // 바로 Ready queue tail에 삽입
    }
}
void TaskB(void) {
	uint32_t b=0;
    while (1) {
        printf("[Tick %u] TaskB run &b=%u\r\n", Kernel_Get_SysTick(), &b);
        Kernel_Delay(2000);
        printf("[Tick %u] TaskB after delay\r\n", Kernel_Get_SysTick());
        Kernel_Yield();
    }
}

void Task_Stack_Test(void)
{
    printf("Task SOF\r\n");
    // char temp[1024] = {0};
    Kernel_Terminate();
}
