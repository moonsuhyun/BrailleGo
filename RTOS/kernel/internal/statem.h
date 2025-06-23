/*
 * statem.h
 *
 *  Created on: May 27, 2025
 *      Author: moon
 */

#ifndef KERNEL_STATEM_H_
#define KERNEL_STATEM_H_

#include "types.h"

#define NUM_TRANSACTIONS 8

void Kernel_StateM_Transaction(uint32_t task_id, KernelTaskEvent_t event);

//void Kernel_statem_ready_schedule(uint32_t task_id);
//void Kernel_statem_running_yield(uint32_t task_id);
//void Kernel_statem_running_block(uint32_t task_id);
//void Kernel_statem_blocked_unblock(uint32_t task_id);
//void Kernel_statem_running_suspend(uint32_t task_id);
//void Kernel_statem_suspended_resume(uint32_t task_id);

#endif /* KERNEL_STATEM_H_ */
