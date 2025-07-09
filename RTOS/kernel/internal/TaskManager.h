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

const Task* Kernel_Task_Get_Current_Task(void);
void Kernel_Task_SysTick_Callback(void);
void Kernel_Task_Scheduler(void);

#ifdef __cplusplus
}
#endif

#endif /* KERNEL_INTERNAL_TASKMANAGER_H_ */
