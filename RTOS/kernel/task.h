#ifndef KERNEL_TASK_H_
#define KERNEL_TASK_H_

#include "memory.h"

#include "types.h"
#include "BspMemoryMap.h"
#include "switch.h"
#include "taskq.h"
#include "statem.h"





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

typedef void (*KernelTaskFunc_t)(void);

void Kernel_task_init(void);
void Kernel_task_start(void);
uint32_t Kernel_task_create(KernelTaskFunc_t startFunc);
void Kernel_task_scheduler(void);

uint32_t Kernel_task_get_current_task_id(void);
void Kernel_task_set_current_task_id(uint32_t task_id);

uint32_t Kernel_task_get_state(uint32_t task_id);
void Kernel_task_set_state(uint32_t task_id, KernelTaskState_t state);

void Kernel_task_yield(uint32_t task_id);
void Kernel_task_resume(uint32_t task_id);

void Kernel_systick_callback(void);

#endif // KERNEL_TASK_H_
