/*
 * taskq.c
 *
 *  Created on: May 27, 2025
 *      Author: moon
 */


#include "taskq.h"
#include "TaskManager.h"


static KernelTaskCirQ_t sTaskQ[TASK_STATE_NUM];

static inline uint32_t sGet_Physical_Index(KernelTaskState_t q_name, uint32_t index);

void Kernel_TaskQ_Init(void) {
    for (uint32_t i=0; i<TASK_STATE_NUM; i++) {
    	sTaskQ[i].front = 0;
    	sTaskQ[i].rear = 0;
        memset(sTaskQ[i].Queue, 0, MAX_TASK_NUM * sizeof(uint32_t));
    }
}

bool Kernel_TaskQ_Is_Empty(KernelTaskState_t q_name) {
    if (q_name >= TASK_STATE_NUM) return false;
    if (sTaskQ[q_name].front == sTaskQ[q_name].rear) return true;
    return false;
}

bool Kernel_TaskQ_Is_Full(KernelTaskState_t q_name) {
    if (q_name >= TASK_STATE_NUM) return false;
    if (((sTaskQ[q_name].rear+1)%MAX_TASK_NUM) == sTaskQ[q_name].front) return true;
    return false;
}

bool Kernel_TaskQ_Enqueue(KernelTaskState_t q_name, uint32_t task_id) {
    if (q_name >= TASK_STATE_NUM) return false;
    if (Kernel_TaskQ_Is_Full(q_name)) return false;


    uint32_t idx = sTaskQ[q_name].rear++;
    sTaskQ[q_name].rear %= MAX_TASK_NUM;

    sTaskQ[q_name].Queue[idx] = task_id;

    sTaskQ[q_name].size++;

    return true;
}

bool Kernel_TaskQ_Dequeue(KernelTaskState_t q_name, uint32_t* task_id) {
    if (q_name >= TASK_STATE_NUM) return false;
    if (Kernel_TaskQ_Is_Empty(q_name)) return false;

    uint32_t idx = sTaskQ[q_name].front++;
    sTaskQ[q_name].front %= MAX_TASK_NUM;

    if (task_id) *task_id = sTaskQ[q_name].Queue[idx];
    sTaskQ[q_name].Queue[idx] = 0;

    sTaskQ[q_name].size--;

    return true;
}

bool Kernel_TaskQ_Get_Front(KernelTaskState_t q_name, uint32_t* task_id) {
	if (q_name >= TASK_STATE_NUM) return false;
	if (Kernel_TaskQ_Is_Empty(q_name)) return false;

	uint32_t idx = sTaskQ[q_name].front;
	if (task_id) *task_id = sTaskQ[q_name].Queue[idx];

	return true;
}

bool Kernel_TaskQ_Iterator_Init(TaskQIterator_t* iter, KernelTaskState_t q_name, uint32_t* task_id) {
	if (task_id == NULL || iter == NULL) return false;
	if (q_name >= TASK_STATE_NUM) return false;
	if (Kernel_TaskQ_Is_Empty(q_name)) return false;
	if (!Kernel_TaskQ_Snapshot(q_name, iter->snapshot)) return false;

	iter->index = 0;
    iter->size = sTaskQ[q_name].size;
	iter->return_value = task_id;

	iter->is_initialized = true;
	return true;
}

bool Kernel_TaskQ_Iterator_Get(TaskQIterator_t* iter) {
	if (iter == NULL) return false;
	if (iter->is_initialized && iter->index < iter->size) {
		*(iter->return_value) = iter->snapshot[iter->index++];
		return true;
	} else {
		return false;
	}
}

bool Kernel_TaskQ_Is_Exist(KernelTaskState_t q_name, uint32_t task_id) {
	if (q_name >= TASK_STATE_NUM) return false;
	if (Kernel_TaskQ_Is_Empty(q_name)) return false;
	TaskQIterator_t iter;
	uint32_t iter_output;
	if (Kernel_TaskQ_Iterator_Init(&iter, q_name, &iter_output)) {
		while (Kernel_TaskQ_Iterator_Get(&iter)) {
			if (iter_output == task_id) return true;
		}
	}
	return false;
}

bool Kernel_TaskQ_Remove(KernelTaskState_t q_name, uint32_t task_id) {
	if (q_name >= TASK_STATE_NUM) return false;
	if (Kernel_TaskQ_Is_Empty(q_name)) return false;
	uint32_t new_queue[MAX_TASK_NUM] = {0};
	uint32_t snapshot[MAX_TASK_NUM] = {0};
	uint32_t new_size = 0;
	if (Kernel_TaskQ_Snapshot(q_name, snapshot)) {
		for (uint32_t i = 0; i < sTaskQ[q_name].size; i++) {
			uint32_t item = snapshot[i];
			if (item != task_id) {
				new_queue[new_size++] = item;
			}
		}
		for (uint32_t i = 0; i < new_size; i++) {
			sTaskQ[q_name].Queue[i] = new_queue[i];
		}
		sTaskQ[q_name].front = 0;
		sTaskQ[q_name].rear = new_size % MAX_TASK_NUM;
		sTaskQ[q_name].size = new_size;
		return true;
	}
	return false;
}

bool Kernel_TaskQ_Snapshot(KernelTaskState_t q_name, uint32_t snapshot[]) {
	if (q_name >= TASK_STATE_NUM) return false;
	if (Kernel_TaskQ_Is_Empty(q_name)) return false;
	if (snapshot == NULL) return false;

	uint32_t q_idx = sTaskQ[q_name].front;
	uint32_t size = sTaskQ[q_name].size;
	for (uint32_t i = 0; i < size; i++) {
		snapshot[i] = sTaskQ[q_name].Queue[q_idx];
		q_idx = (q_idx+1) % MAX_TASK_NUM;
	}
	return true;
}

bool Kernel_TaskQ_Enqueue_Sorted_By_Wake_Time(KernelTaskState_t q_name, uint32_t task_id)
{
	if (q_name >= TASK_STATE_NUM) return false;
	if (Kernel_TaskQ_Is_Full(q_name)) return false;

	uint32_t wake_time = Kernel_Task_Get_Wake_Time_By_Id(task_id);

	int32_t position = 0;
	for (; position < sTaskQ[q_name].size; position++)
	{
		uint32_t currrent_id = sTaskQ[q_name].Queue[sGet_Physical_Index(q_name, position)];
		uint32_t current_wake_time = Kernel_Task_Get_Wake_Time_By_Id(currrent_id);

		if (current_wake_time >= wake_time) break;
	}

	for (int32_t i = sTaskQ[q_name].size - 1; i >= position; i--)
		sTaskQ[q_name].Queue[sGet_Physical_Index(q_name, i + 1)] = sTaskQ[q_name].Queue[sGet_Physical_Index(q_name, i)];

	sTaskQ[q_name].Queue[sGet_Physical_Index(q_name, position)] = task_id;

	sTaskQ[q_name].size++;
	sTaskQ[q_name].rear = (sTaskQ[q_name].front + sTaskQ[q_name].size) % MAX_TASK_NUM;

	return true;
}

uint32_t Kernel_TaskQ_Get_Size(KernelTaskState_t q_name)
{
	return sTaskQ[q_name].size;
}

static inline uint32_t sGet_Physical_Index(KernelTaskState_t q_name, uint32_t index)
{
	return (sTaskQ[q_name].front + index) % MAX_TASK_NUM;
}