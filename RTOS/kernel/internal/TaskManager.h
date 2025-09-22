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

// void Kernel_Task_Wrapper(void (*task_func)(void*), void* arg);
const Task* Kernel_Task_Get_Current_Task(void);
int32_t Kernel_Task_Get_Current_Task_Id(void);
void Kernel_Task_SysTick_Callback(void);
void Kernel_Task_Scheduler(void);
uint32_t Kernel_Task_Get_Wake_Time_By_Id(uint32_t id);

#ifdef __cplusplus
}
#endif

#endif /* KERNEL_INTERNAL_TASKMANAGER_H_ */
