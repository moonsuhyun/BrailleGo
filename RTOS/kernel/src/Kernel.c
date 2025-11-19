//
// Created by ��������� on 25. 10. 20..
//

#include "Kernel.h"

#include "BspHwInit.h"
#include "BspSysTick.h"
#include "PortCore.h"
#include "PortTask.h"
#include "TaskManager.h"

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

bool Mutex_Lock(MutexType_t mutex_type)
{
    Port_Core_SVC_Call(SC_MUTLOCK, (void*)mutex_type, NULL, NULL);
}

bool Mutex_Unlock(MutexType_t mutex_type)
{
    Port_Core_SVC_Call(SC_MUTUNLOCK, (void*)mutex_type, NULL, NULL);
}

uint32_t SysTick_GetTick(void)
{
    return BSP_Get_Tick();
}

bool Kernel_Is_Running(void)
{
    return is_initiated;
}
