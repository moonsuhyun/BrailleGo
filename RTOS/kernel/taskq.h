/*
 * taskq.h
 *
 *  Created on: May 27, 2025
 *      Author: moon
 */

#ifndef KERNEL_TASKQ_H_
#define KERNEL_TASKQ_H_

#include "types.h"
#include "task.h"


void Kernel_taskq_init(void);
bool Kernel_taskq_is_empty(KernelTaskState_t q_name);
bool Kernel_taskq_is_full(KernelTaskState_t q_name);
bool Kernel_taskq_enqueue(KernelTaskState_t q_name, uint32_t task_id);
bool Kernel_taskq_dequeue(KernelTaskState_t q_name, uint32_t* task_id);

#endif /* KERNEL_TASKQ_H_ */
