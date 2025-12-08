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
            Busy(1000);   // 1sec 바쁜 대기
        }

        printf("[L] unlock uart\r\n");
        Mutex_Unlock(MUTEX_UART);

        // 3) 한참 쉬었다가 다시 반복 (L 주기 크게)
        Task_Delay(30000);     // 30sec
    }
}

void Task_M(void* arg)
{
    (void)arg;
    for (;;) {
        // L이 뮤텍스를 잡은 지 조금 지난 시점에 먼저 깨어남
        printf("[M] delay 1sec tick=%u\r\n", SysTick_GetTick());
        Task_Delay(1000);  // 1sec마다 깨어남 (H보다 먼저)

        printf("[M] wake, busy work start tick=%u\r\n", SysTick_GetTick());

        Busy(2000);      // 2sec CPU만 먹음 (뮤텍스 안 씀)

        printf("[M] busy work end tick=%u\r\n", SysTick_GetTick());
    }
}

void Task_H(void* arg)
{
    (void)arg;
    for (;;) {
        // M이 먼저 한 번 깨어나서 CPU를 먹고 있는 동안
        // 그보다 조금 늦게 등장해서 뮤텍스를 요구
        printf("[H] delay 2sec tick=%u\r\n", SysTick_GetTick());
        Task_Delay(2000);   // 2sec 후에 등장 (M보다 나중, L 임계구역 안)

        printf("[H] try lock uart tick=%u\r\n", SysTick_GetTick());

        // 여기서 BLOCKED 될 수 있고, 그 시점에 우선순위 상속 발생
        Mutex_Lock(MUTEX_UART);

        printf("[H] in critical section tick=%u\r\n", SysTick_GetTick());
        Busy(1000);  // 1sec 정도 임계구역

        printf("[H] unlock uart tick=%u\r\n", SysTick_GetTick());
        Mutex_Unlock(MUTEX_UART);

        // 다시는 한동안 안 나오게 길게 쉼 (패턴이 눈에 잘 보이도록)
        Task_Delay(100000); // 100sec 정도
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