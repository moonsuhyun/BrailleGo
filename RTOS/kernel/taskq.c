/*
 * taskq.c
 *
 *  Created on: May 27, 2025
 *      Author: moon
 */

#include "taskq.h"

KernelTaskCirQ_t taskQ[TASK_STATE_NUM];

void Kernel_taskq_init(void) {
    for (uint32_t i=0; i<TASK_STATE_NUM; i++) {
    	taskQ[i].front = 0;
    	taskQ[i].rear = 0;
        memset(taskQ[i].Queue, 0, MAX_TASK_NUM * sizeof(uint32_t));
    }
}

bool Kernel_taskq_is_empty(KernelTaskState_t q_name) {
    if (q_name >= TASK_STATE_NUM) return false;
    if (taskQ[q_name].front == taskQ[q_name].rear) return true;
    return false;
}

bool Kernel_taskq_is_full(KernelTaskState_t q_name) {
    if (q_name >= TASK_STATE_NUM) return false;
    if (((taskQ[q_name].rear+1)%MAX_TASK_NUM) == taskQ[q_name].front) return true;
    return false;
}

bool Kernel_taskq_enqueue(KernelTaskState_t q_name, uint32_t task_id) {
    if (q_name >= TASK_STATE_NUM) return false;
    if (Kernel_taskq_is_full(q_name)) return false;

    taskQ[q_name].rear++;
    taskQ[q_name].rear %= MAX_TASK_NUM;

    uint32_t idx = taskQ[q_name].rear;
    taskQ[q_name].Queue[idx] = task_id;

    return true;
}

bool Kernel_taskq_dequeue(KernelTaskState_t q_name, uint32_t* task_id) {
    if (q_name >= TASK_STATE_NUM) return false;
    if (Kernel_taskq_is_empty(q_name)) return false;

    taskQ[q_name].front++;
    taskQ[q_name].front %= MAX_TASK_NUM;

    uint32_t idx = taskQ[q_name].front;
    *task_id = taskQ[q_name].Queue[idx];

    taskQ[q_name].Queue[idx] = 0;

    return true;
}
