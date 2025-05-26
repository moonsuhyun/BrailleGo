#ifndef KERNEL_TASK_H_
#define KERNEL_TASK_H_

#include "stdint.h"
#include "stdbool.h"
#include "memory.h"

#include "BspMemoryMap.h"
#include "switch.h"

#define NOT_ENOUGH_TASK_NUM 0xFFFFFFFF
#define MAX_TASK_NUM (TASK_STACK_AREA_SIZE / TASK_STACK_SIZE)
#define TIME_SLICE_CNT 10

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

typedef enum KernelTaskState_t {
	TASK_READY,
	TASK_RUNNING,
	TASK_BLOCKED,
	TASK_TERMINATED
} KernelTaskState_t;

// 커널에서 관리하는 Tack Control Block의 구조체
typedef struct KernelTcb_t {
    uint32_t* sp;
    uint8_t* stack_base;
    uint8_t priority;
    KernelTaskState_t state;
} KernelTcb_t;

typedef void (*KernelTaskFunc_t)(void);

void Kernel_task_init(void);
void Kernel_task_start(void);
uint32_t Kernel_task_create(KernelTaskFunc_t startFunc);
void Kernel_task_scheduler(void);
uint32_t Kernel_task_get_current_task_id(void);

void Kernel_systick_callback(void);

#endif // KERNEL_TASK_H_
