#ifndef KERNEL_TASK_H_
#define KERNEL_TASK_H_

#include "stdio.h"
#include "devio.h"

#include "memory.h"

#include "types.h"
#include "BspMemoryMap.h"
#include "switch.h"
#include "taskq.h"
#include "statem.h"


#define IDLE_TASK_ID 0


//// Context switching시 stack에 백업할 레지스터의 구조
//typedef struct KernelExceptionStackFrame_t {
//	uint32_t r0_r3[4];
//	uint32_t r12;
//	uint32_t lr;
//	uint32_t pc;
//	uint32_t xpsr;
//} KernelExceptionStackFrame_t;
//
//typedef struct KernelTaskStackFrame_t {
//    uint32_t r4_r11[8];
//    KernelExceptionStackFrame_t exc_frame;
//} KernelTaskStackFrame_t;

typedef void (*KernelTaskFunc_t)(void);

void Kernel_Task_Init(void);
void Kernel_Task_Start(void);
uint32_t Kernel_Task_Create(KernelTaskFunc_t start_func);
void Kernel_Task_Yield(uint32_t task_id);
void Kernel_Task_Delay(uint32_t task_id, uint32_t ms);

void Kernel_Task_Scheduler(void);


uint32_t Kernel_Task_Get_Current_Task_Id(void);
void Kernel_Task_Set_Current_Task_Id(uint32_t task_id);

uint32_t Kernel_Task_Get_State(uint32_t task_id);
void Kernel_Task_Set_State(uint32_t task_id, KernelTaskState_t state);

KernelTcb_t* Kernel_Task_Get_Current_Tcb(void);

void Kernel_Task_SysTick_Callback(void);

#endif // KERNEL_TASK_H_
