//
// Created by ��������� on 25. 10. 20..
//

#include "Kernel.h"

#include "BspHwInit.h"
#include "BspSysTick.h"
#include "BspUart.h"
#include "PortCore.h"
#include "PortTask.h"
#include "TaskManager.h"
#include <stdio.h>

static bool is_initiated = false;

void Kernel_Start(KernelTaskFunc_t init_task)
{
    if (!is_initiated)
    {
        /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
        HAL_Init();

        /* Configure the system clock */
        SystemClock_Config();

        /* Initialize all configured peripherals */
        MX_GPIO_Init();
        MX_USART2_UART_Init();

        TaskManager_Init(init_task);

        Port_Core_Interrupt_Init();

        BSP_UART_Init();

        is_initiated = true;

        Port_Task_Start();
    }
}

uint32_t Task_Create(void(* start_func)(void*), void* arg, uint32_t priority)
{
    return Port_Core_SVC_Call(SC_CREATE, (void*)start_func, (void*)arg,  (void*)priority);
}

void Task_Yield(void)
{
    Port_Core_SVC_Call(SC_YIELD, NULL, NULL, NULL);
}

void Task_Delay(uint32_t ms)
{
    Port_Core_SVC_Call(SC_DELAY, (void*)ms, NULL, NULL);
}

void Task_Terminate()
{
    Port_Core_SVC_Call(SC_TERMINATE, NULL, NULL, NULL);
}

void Mutex_Lock(MutexType_t mutex_type)
{
    Port_Core_SVC_Call(SC_MUTLOCK, (void*)mutex_type, NULL, NULL);
}

void Mutex_Unlock(MutexType_t mutex_type)
{
    Port_Core_SVC_Call(SC_MUTUNLOCK, (void*)mutex_type, NULL, NULL);
}

SemHandle_t Semaphore_Create(int32_t initial_count)
{
    return Port_Core_SVC_Call(SC_SEMCREATE, (void*)initial_count, NULL, NULL);
}

uint32_t Semaphore_Wait(SemHandle_t h)
{
    return Port_Core_SVC_Call(SC_SEMWAIT, (void*)h, NULL, NULL);
}

uint32_t Semaphore_Signal(SemHandle_t h)
{
    return Port_Core_SVC_Call(SC_SEMSIGNAL, (void*)h, NULL, NULL);
}

int32_t Semaphore_GetCount(SemHandle_t h)
{
    return Port_Core_SVC_Call(SC_SEMGETCNT, (void*)h, NULL, NULL);
}

uint32_t SysTick_GetTick(void)
{
    return BSP_Get_Tick();
}

bool Kernel_Is_Running(void)
{
    return is_initiated;
}

int32_t UART_Read(uint8_t* buf, uint32_t len)
{
    uint8_t ch;
    for (size_t i = 0; i < len; ++i) {
        if (BSP_UART_GetCharBlocking(&ch) < 0) {
            return -1;
        }
        buf[i] = ch;
    }
    return 0;
}

int32_t UART_ReadLine(char* buf, uint32_t max_len)
{
    size_t pos = 0;
    uint8_t ch;

    if (max_len == 0) return -1;

    for (;;) {
        if (BSP_UART_GetCharBlocking(&ch) < 0) {
            continue;
        }

        // echo
        if (ch == '\r' || ch == '\n') {
            // CR/LF
            Mutex_Lock(MUTEX_UART);
            printf("\r\n");
            Mutex_Unlock(MUTEX_UART);
            break;
        } else if (ch == '\b' || ch == 0x7F) {
            // backspace
            if (pos > 0) {
                pos--;
                Mutex_Lock(MUTEX_UART);
                printf("\b \b");
                Mutex_Unlock(MUTEX_UART);
            }
        } else {
            if (pos < max_len - 1) {
                buf[pos++] = (char)ch;
                Mutex_Lock(MUTEX_UART);
                printf("%c", ch);   // echo
                Mutex_Unlock(MUTEX_UART);
            } else {
                // buffer full
            }
        }
    }

    buf[pos] = '\0';
    return (int)pos;
}


int32_t UART_ReadLineEx(char* buf, uint32_t max_len,
                        void (*begin)(void),
                        void (*end)(void))
{
    static uint8_t drop_lf = 0;

    size_t pos = 0;
    uint8_t ch;
    uint8_t started = 0;

    for (;;) {
        BSP_UART_GetCharBlocking(&ch);

        if (drop_lf) {
            drop_lf = 0;
            if (ch == '\n') continue;
        }

        if (!started) {
            begin();
            Mutex_Lock(MUTEX_UART);
            started = 1;
        }

        if (ch == '\r' || ch == '\n') {
            if (ch == '\r') drop_lf = 1;
            printf("\r\n");
            Mutex_Unlock(MUTEX_UART);
            end();
            break;
        } else if (ch == '\b' || ch == 0x7F) {
            // backspace
            if (pos > 0) {
                pos--;
                printf("\b \b");
            }
        } else {
            if (pos < max_len - 1) {
                buf[pos++] = (char)ch;
                printf("%c", ch);   // echo
            } else {
                // buffer full
            }
        }
    }
    buf[pos] = '\0';
    return (int)pos;
}
