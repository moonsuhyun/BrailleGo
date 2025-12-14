#include <Kernel.h>

#include <stdio.h>

#include "usertask.h"

static void sTask_Init();

int main(void)
{
    Kernel_Start(sTask_Init);

    while (1)
    {
    }
}

static void sTask_Init(void)
{
    Task_Create(Task_Inference, NULL, 2);
    Task_Create(Task_Heartbeat, NULL, 1);
    Task_Create(Task_Shell, NULL, 0);

    while (1)
    {
        Task_Yield();
    }
}
