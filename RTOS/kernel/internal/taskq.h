/*
 * taskq.h
 *
 *  Created on: May 27, 2025
 *      Author: moon
 */

#ifndef KERNEL_TASKQ_H_
#define KERNEL_TASKQ_H_

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

void Kernel_TaskQ_Init(void);
bool Kernel_TaskQ_Is_Empty(KernelTaskState_t q_name);
bool Kernel_TaskQ_Is_Full(KernelTaskState_t q_name);
bool Kernel_TaskQ_Enqueue(KernelTaskState_t q_name, uint32_t task_id);
bool Kernel_TaskQ_Dequeue(KernelTaskState_t q_name, uint32_t* task_id);
bool Kernel_TaskQ_Get_Front(KernelTaskState_t q_name, uint32_t* task_id);
bool Kernel_TaskQ_Remove(KernelTaskState_t q_name, uint32_t task_id);

bool Kernel_TaskQ_Iterator_Init(TaskQIterator_t* iter, KernelTaskState_t q_name, uint32_t* task_id);
bool Kernel_TaskQ_Iterator_Get(TaskQIterator_t* iter);
bool Kernel_TaskQ_Snapshot(KernelTaskState_t q_name, uint32_t snapshot[]);

#ifdef __cplusplus
}
#endif


#endif /* KERNEL_TASKQ_H_ */
