/*
 * TaskManager.h
 *
 *  Created on: Jul 8, 2025
 *      Author: moon
 */

#ifndef KERNEL_INTERNAL_TASKMANAGER_H_
#define KERNEL_INTERNAL_TASKMANAGER_H_


// C, ASM에서 호출하기 위한 함수 Wrapper
#ifdef __cplusplus
extern "C" {
#endif

typedef struct Task Task;
typedef void (*KernelTaskFunc_t)(void*);

void TaskManager_Init(KernelTaskFunc_t init_task);
int32_t TaskManager_Create(KernelTaskFunc_t start_func, void* arg, uint32_t priority);
int32_t TaskManager_Yield();
int32_t TaskManager_Delay(uint32_t ms);
int32_t TaskManager_Terminate();

Task* TaskManager_Get_Current_Task(void);
int32_t TaskManager_Get_Current_Task_Id(void);
void TaskManager_SysTick_Callback(void);
void TaskManager_Scheduler(void);
uint32_t Kernel_Task_Get_Wake_Time_By_Id(uint32_t id);
int32_t Kernel_Task_Terminate(void);

#ifdef __cplusplus
}
#endif

#endif /* KERNEL_INTERNAL_TASKMANAGER_H_ */
