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

static void Busy(int32_t ms)
{
    int32_t start = SysTick_GetTick();
    while (((int32_t)SysTick_GetTick() - start) < ms) {
        __asm volatile("nop");
    }
}

void Task_L(void* arg)
{
    (void) arg;
    for (;;) {
        // 1) UART 뮤텍스 잡기
        Mutex_Lock(MUTEX_UART);
        printf("[L] lock uart at tick=%u\r\n", SysTick_GetTick());

        // 2) 임계구역에서 오래 일하기 (대략 5 * 10ms = 50ms)
        for (int i = 0; i < 5; ++i) {
            printf("[L] logging i=%d  tick=%u\r\n", i, SysTick_GetTick());
            Busy(10);   // 10ms 정도 바쁜 대기
        }

        printf("[L] unlock uart\r\n");
        Mutex_Unlock(MUTEX_UART);

        // 3) 한참 쉬었다가 다시 반복
        Task_Delay(200);     // 200ms 정도
    }
}

void Task_M(void* arg)
{
    (void)arg;
    for (;;) {
        Task_Delay(15);  // 15ms마다 깨어나서

        printf("[M] wake, busy work start tick=%u\r\n", SysTick_GetTick());

        Busy(20);      // 약 20ms CPU만 먹음 (뮤텍스 안 씀)

        printf("[M] busy work end tick=%u\r\n", SysTick_GetTick());
    }
}

void Task_H(void* arg)
{
    (void)arg;
    for (;;) {
        // L이 UART 뮤텍스 잡고 있는 타이밍까지 조금 기다렸다가 등장
        Task_Delay(20);

        printf("[H] try lock uart tick=%u\r\n", SysTick_GetTick());

        // 여기서 BLOCKED 될 수 있고, 그 시점에 우선순위 상속 발생
        Mutex_Lock(MUTEX_UART);

        printf("[H] in critical section tick=%u\r\n", SysTick_GetTick());
        Busy(10);  // 10ms 정도 임계구역

        printf("[H] unlock uart tick=%u\r\n", SysTick_GetTick());
        Mutex_Unlock(MUTEX_UART);

        Task_Delay(500); // 다시 한참 쉼
    }

}


void TaskA(void* arg)
{
    while (1)
    {
        Mutex_Lock(MUTEX_UART);
        printf("[TaskA] tick=%u\r\n", SysTick_GetTick());
        Mutex_Unlock(MUTEX_UART);
        Task_Delay(10);
    }

}
void TaskB(void* arg)
{
    while (1) {
        Mutex_Lock(MUTEX_UART);
        printf("[TaskB] tick=%u\r\n", SysTick_GetTick());
        Mutex_Unlock(MUTEX_UART);
        Task_Delay(20);
    }
}