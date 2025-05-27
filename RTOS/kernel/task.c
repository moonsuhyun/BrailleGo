
#include "task.h"
#include "cmsis_gcc.h"

static KernelTcb_t sTask_list[MAX_TASK_NUM];
static uint32_t sAllocated_tcb_count;

// round robin scheduler
static uint32_t sCurrent_task_id;
static void Scheduler_round_robin_algorithm(void);

KernelTcb_t* gCurrent_tcb;
//KernelTcb_t* Next_tcb;

void Kernel_task_init(void) {
    sAllocated_tcb_count = 1;
    sCurrent_task_id = 1;

    Kernel_taskq_init();

    for(uint32_t i = 0; i < MAX_TASK_NUM; i++) {
    	sTask_list[i].sp = (uint32_t*)(TASK_STACK_TOP - i * TASK_STACK_SIZE);
        sTask_list[i].stack_base = (uint8_t*)(TASK_STACK_TOP - (i + 1) * TASK_STACK_SIZE);
        sTask_list[i].sp -= sizeof(TaskStackFrame_t) / 4;
        memset(sTask_list[i].stack_base, 0, TASK_STACK_SIZE);
        sTask_list[i].state = TASK_TERMINATED;
    }
}

// 스케쥴러 최초 동작 시 context 백업 안 함
//void Kernel_task_start(void) {
//    Next_tcb = &sTask_list[sCurrent_tcb_index];
//    __set_CONTROL(__get_CONTROL() | 0x2); // use PSP
//        asm volatile (
//			"LDR   r1, =Next_tcb   \n"
//			"LDR   r1, [r1]        \n"
//			"LDR   r0, [r1]        \n"
//			"LDMIA r0!, {r4-r11}   \n"
//			"MSR   psp, r0         \n"
//            "MOV   lr, #0xFFFFFFFD \n" // EXC_RETURN for thread mode
//            "BX    lr              \n"
//        );
//    // Restore_context();
//}
void Kernel_task_start(void) {
	uint32_t task_id;
    Kernel_taskq_dequeue(TASK_READY, &task_id);
    Kernel_statem_transaction(task_id, EVENT_RESUME);
    Port_task_start();
}

uint32_t Kernel_task_create(KernelTaskFunc_t startFunc) {
	uint32_t task_id = sAllocated_tcb_count++;

    if(sAllocated_tcb_count > MAX_TASK_NUM) return NOT_ENOUGH_TASK_NUM;

    KernelTcb_t* new_tcb = &sTask_list[task_id];

    TaskStackFrame_t* task_frame = (TaskStackFrame_t*) new_tcb->sp;
    uint32_t pc = (uint32_t)startFunc;
    Port_task_create(task_frame, pc);

    new_tcb->state = TASK_READY;
    Kernel_taskq_enqueue(TASK_READY, task_id);

    return task_id;
}

// Context switching 시 PendSV에서 호출
void Kernel_task_scheduler(void) {
    Scheduler_round_robin_algorithm();
}

void Scheduler_round_robin_algorithm(void) {
	uint32_t task_id;
	Kernel_taskq_dequeue(TASK_READY, &task_id);
	Kernel_task_resume(task_id);
}

uint32_t Kernel_task_get_current_task_id(void) {
    return sCurrent_task_id;
}

void Kernel_task_set_current_task_id(uint32_t task_id) {
	sCurrent_task_id = task_id;
	gCurrent_tcb = &sTask_list[sCurrent_task_id];
}

uint32_t Kernel_task_get_state(uint32_t task_id) {
	return sTask_list[task_id].state;
}

void Kernel_task_set_state(uint32_t task_id, KernelTaskState_t state) {
	sTask_list[task_id].state = state;
}

void Kernel_task_yield(uint32_t task_id) {
	Kernel_statem_transaction(task_id, EVENT_YIELD);
}

void Kernel_task_resume(uint32_t task_id) {
	Kernel_statem_transaction(task_id, EVENT_RESUME);
}


void Kernel_systick_callback(void) {
//	static int tick = 0;
//	tick++;
//	if (tick > TIMESLICE_CNT) {
//		Kernel_task_scheduler();
//	}
}

void action(void) {

}
