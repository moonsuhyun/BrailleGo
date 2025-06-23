
#include "task.h"

#include "PortTask.h"
#include "stdio.h"
#include "devio.h"
#include "memory.h"
#include "BspMemoryMap.h"
#include "taskq.h"
#include "statem.h"
#include "BspSysTick.h"

static KernelTcb_t sTask_list[MAX_TASK_NUM];
static uint32_t sAllocated_tcb_count;
static uint32_t sIdle_task_id;

static uint32_t sCurrent_task_id;
static uint32_t sScheduler_Round_Robin_Algorithm(void);
static void sEvent_Yield(uint32_t task_id);
static void sEvent_Schedule(uint32_t task_id);
static void sEvent_Delay(uint32_t task_id);
static void sEvent_Unblock(uint32_t task_id);
static void sEvent_Terminate(uint32_t task_id);
static void sEvent_Create(uint32_t task_id);

static void sIdle_Task(void);

//KernelTcb_t* gCurrent_tcb; Global 변수 사용 최소화

// 태스크 생성, 관리 등

void Kernel_Task_Init(void) {
    sAllocated_tcb_count = 0;
    sCurrent_task_id = 0;

    Kernel_TaskQ_Init();

    for(uint32_t i = 0; i < MAX_TASK_NUM; i++) {
    	sTask_list[i].sp = (uint32_t*)(TASK_STACK_TOP - i * TASK_STACK_SIZE);
        sTask_list[i].stack_base = (uint8_t*)(TASK_STACK_TOP - (i + 1) * TASK_STACK_SIZE);
        sTask_list[i].sp -= sizeof(TaskStackFrame_t) / 4;
        memset(sTask_list[i].stack_base, 0, TASK_STACK_SIZE);
        sTask_list[i].delay_until_time = 0;
        sTask_list[i].state = TASK_TERMINATED;
    }
    sIdle_task_id = Kernel_Task_Create(sIdle_Task);
}

void Kernel_Task_Start(void) {
	Kernel_StateM_Transaction(sIdle_task_id, EVENT_SCHEDULE);
    Port_Task_Start();
}

uint32_t Kernel_Task_Create(KernelTaskFunc_t start_func) {
	uint32_t task_id = sAllocated_tcb_count++;

    if(sAllocated_tcb_count > MAX_TASK_NUM) return NOT_ENOUGH_TASK_NUM;

    KernelTcb_t* new_tcb = &sTask_list[task_id];

    TaskStackFrame_t* task_frame = (TaskStackFrame_t*) new_tcb->sp;
    uint32_t pc = (uint32_t)start_func;
    Port_Task_Create(task_frame, pc);

//    new_tcb->state = TASK_READY;
//    if (task_id != sIdle_task_id) {
//    	Kernel_TaskQ_Enqueue(TASK_READY, task_id);
//    }
    sEvent_Create(task_id);

    return task_id;
}

void Kernel_Task_Yield(uint32_t task_id) {
	sEvent_Yield(task_id);
}

void Kernel_Task_Delay(uint32_t task_id, uint32_t ms) {
	uint32_t start_tick = BSP_Get_Tick();
	sTask_list[task_id].delay_until_time = start_tick + ms;
	sEvent_Delay(task_id);
}

void Kernel_Task_Terminate(uint32_t task_id) {
	sEvent_Terminate(task_id);
}

// Context switching 시 PendSV에서 호출
void Kernel_Task_Scheduler(void) {
	uint32_t task_id;
	if (Kernel_TaskQ_Is_Empty(TASK_READY)) {
		task_id = sIdle_task_id;
		printf("[Tick %u] Idle task(id=%u) scheduled\r\n", BSP_Get_Tick(), task_id);
	} else {
		task_id = sScheduler_Round_Robin_Algorithm();
	}
    sEvent_Schedule(task_id);
}

// getter, setter
uint32_t Kernel_Task_Get_Idle_Task_Id(void) {
	return sIdle_task_id;
}
uint32_t Kernel_Task_Get_Current_Task_Id(void) {
    return sCurrent_task_id;
}

void Kernel_Task_Set_Current_Task_Id(uint32_t task_id) {
	sCurrent_task_id = task_id;
}

uint32_t Kernel_Task_Get_State(uint32_t task_id) {
	return sTask_list[task_id].state;
}

void Kernel_Task_Set_State(uint32_t task_id, KernelTaskState_t state) {
	sTask_list[task_id].state = state;
}

KernelTcb_t* Kernel_Task_Get_Current_Tcb(void) {
	return &sTask_list[sCurrent_task_id];
}

void Kernel_Task_SysTick_Callback(void) {
	if (!Kernel_TaskQ_Is_Empty(TASK_BLOCKED_DELAY)) {
		TaskQIterator_t iter;
		uint32_t task_id;
		if (Kernel_TaskQ_Iterator_Init(&iter, TASK_BLOCKED_DELAY, &task_id)){
			while (Kernel_TaskQ_Iterator_Get(&iter)) {
				if (sTask_list[task_id].delay_until_time <= BSP_Get_Tick()) {
					Kernel_TaskQ_Remove(TASK_BLOCKED_DELAY, task_id);
					sEvent_Unblock(task_id);
				}
			}
		}
	}
}


// static functions

uint32_t sScheduler_Round_Robin_Algorithm(void) {
	uint32_t task_id;
	Kernel_TaskQ_Dequeue(TASK_READY, &task_id);
	return task_id;
}

// task event 관련

void sEvent_Yield(uint32_t task_id) {
	Kernel_StateM_Transaction(task_id, EVENT_YIELD);
}

void sEvent_Schedule(uint32_t task_id) {
	Kernel_StateM_Transaction(task_id, EVENT_SCHEDULE);
}

void sEvent_Delay(uint32_t task_id) {
	Kernel_StateM_Transaction(task_id, EVENT_DELAY);
}

void sEvent_Unblock(uint32_t task_id) {
	Kernel_StateM_Transaction(task_id, EVENT_UNBLOCK);
}

void sEvent_Terminate(uint32_t task_id) {
	Kernel_StateM_Transaction(task_id, EVENT_TERMINATE);
}

void sEvent_Create(uint32_t task_id) {
	Kernel_StateM_Transaction(task_id, EVENT_CREATE);
}


void sIdle_Task(void) {
	Kernel_Task_Yield(sIdle_task_id);
	while (1) {
		if (!Kernel_TaskQ_Is_Empty(TASK_READY)) Kernel_Task_Yield(sIdle_task_id);
		Port_Core_Wait_For_Interrupt();
	}
}

