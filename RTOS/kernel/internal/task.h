#ifndef KERNEL_TASK_H_
#define KERNEL_TASK_H_

#include "types.h"


typedef void (*KernelTaskFunc_t)(void);

void Kernel_Task_Init(void);
void Kernel_Task_Start(void);
uint32_t Kernel_Task_Create(KernelTaskFunc_t start_func);
void Kernel_Task_Yield(uint32_t task_id);
void Kernel_Task_Delay(uint32_t task_id, uint32_t ms);
void Kernel_Task_Terminate(uint32_t task_id);

void Kernel_Task_Scheduler(void);

uint32_t Kernel_Task_Get_Idle_Task_Id(void);
uint32_t Kernel_Task_Get_Current_Task_Id(void);
void Kernel_Task_Set_Current_Task_Id(uint32_t task_id);

uint32_t Kernel_Task_Get_State(uint32_t task_id);
void Kernel_Task_Set_State(uint32_t task_id, KernelTaskState_t state);

KernelTcb_t* Kernel_Task_Get_Current_Tcb(void);

void Kernel_Task_SysTick_Callback(void);

#endif // KERNEL_TASK_H_
