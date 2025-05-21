#include "stm32f1xx.h"

#include "stdint.h"
#include "stdbool.h"
#include "memory.h"

#include "task.h"

static KernelTcb_t sTask_list[MAX_TASK_NUM];
static uint32_t sAllocated_tcb_index;

// round robin scheduler
static uint32_t sCurrent_tcb_index;
static KernelTcb_t* Scheduler_round_robin_algorithm(void);

KernelTcb_t* Current_tcb;
KernelTcb_t* Next_tcb;

void Kernel_task_init(void) {
    sAllocated_tcb_index = 0;
    sCurrent_tcb_index = 0;

    for(uint32_t i = 0; i < MAX_TASK_NUM; i++) {
        sTask_list[i].stack_base = (uint8_t*)(TASK_STACK_TOP - i * TASK_STACK_SIZE);
        sTask_list[i].sp = (uint32_t)sTask_list[i].stack_base + TASK_STACK_SIZE;

        sTask_list[i].sp -= sizeof(KernelTaskStackFrame_t);
    }
}

// 스케쥴러 최초 동작 시 context 백업 안 함
void Kernel_task_start(void) {
    Next_tcb = &sTask_list[sCurrent_tcb_index];
    __set_PSP((uint32_t)Next_tcb->sp);           // PSP 세팅
        asm volatile (
            "LDMIA r0!, {r4-r11}\n"                  // 소프트 컨텍스트 POP
            "MSR psp, r0\n"
            "MOV lr, #0xFFFFFFFD\n"                  // EXC_RETURN for thread mode
            "BX lr\n"
        );
    // Restore_context();
}

uint32_t Kernel_task_create(KernelTaskFunc_t startFunc) {
    KernelTcb_t* new_tcb = &sTask_list[sAllocated_tcb_index++];
    if(sAllocated_tcb_index > MAX_TASK_NUM) return NOT_ENOUGH_TASK_NUM;

    KernelTaskStackFrame_t* task_frame = (KernelTaskStackFrame_t*) new_tcb->sp;

    task_frame->exc_frame.xpsr = 0x01000000UL;
    task_frame->exc_frame.pc = (uint32_t)startFunc;
    task_frame->exc_frame.lr = 0xFFFFFFFDUL;
    task_frame->exc_frame.r12 = 0UL;
    memset(task_frame->exc_frame.r0_r3, 0, sizeof(task_frame->exc_frame.r0_r3));
    memset(task_frame->r4_r11, 0, sizeof(task_frame->r4_r11));

    return (sAllocated_tcb_index - 1);
}

void Kernel_task_scheduler(void) {
    Current_tcb = &sTask_list[sCurrent_tcb_index];
    Next_tcb = Scheduler_round_robin_algorithm();

    Kernel_trigger_pendsv();
}

static KernelTcb_t* Scheduler_round_robin_algorithm(void) {
    sCurrent_tcb_index++;
    sCurrent_tcb_index %= sAllocated_tcb_index;

    return &sTask_list[sCurrent_tcb_index];
}

uint32_t Kernel_task_get_current_task_id(void) {
    return sCurrent_tcb_index;
}

void Kernel_trigger_pendsv(void) {
    SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
}
