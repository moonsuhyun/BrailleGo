/*
 * types.h
 *
 *  Created on: May 27, 2025
 *      Author: moon
 */

#ifndef KERNEL_TYPES_H_
#define KERNEL_TYPES_H_

#include "BspMemoryMap.h"

#define NOT_ENOUGH_TASK_NUM 0xFFFFFFFF
#define MAX_TASK_NUM (TASK_STACK_AREA_SIZE / TASK_STACK_SIZE)
#define TIME_SLICE_CNT 10

#include "stdint.h"
#include "stdbool.h"

typedef enum KernelTaskState {
	TASK_READY,
	TASK_RUNNING,
	TASK_BLOCKED,
	TASK_TERMINATED,
	TASK_STATE_NUM
} KernelTaskState_t;

// 커널에서 관리하는 Tack Control Block의 구조체
typedef struct KernelTcb {
    uint32_t* sp;
    uint8_t* stack_base;
//  uint8_t priority;
    KernelTaskState_t state;
} KernelTcb_t;

typedef enum KernelTaskEvent {
	EVENT_YIELD,
	EVENT_BLOCK,
	EVENT_UNBLOCK,
	EVENT_RESUME,
} KernelTaskEvent_t;

typedef struct KernelTaskCirQ {
    uint32_t front;
    uint32_t rear;
    uint32_t Queue[MAX_TASK_NUM];
} KernelTaskCirQ_t;

typedef struct KernelEventActionTable {
	KernelTaskState_t current_state;
	KernelTaskEvent_t event;
	void (*action)(uint32_t task_id);
	KernelTaskState_t next_state;
} KernelEventActionTable_t;

#endif /* KERNEL_TYPES_H_ */
