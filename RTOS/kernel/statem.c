/*
 * statem.c
 *
 *  Created on: May 27, 2025
 *      Author: moon
 */

#include "statem.h"
#include "stm32f1xx.h"

static void sReady_Schedule(uint32_t task_id);
static void sRunning_Yield(uint32_t task_id);
static void sRunning_Block(uint32_t task_id);
static void sBlocked_Unblock(uint32_t task_id);
static void sRunning_Suspend(uint32_t task_id);
static void sSuspended_Resume(uint32_t task_id);

KernelEventActionTable_t table[NUM_TRANSACTIONS] = {
//		|CUR_STATE------|EVENT-----------|ACTION------------|NEXT_STATE----|
		{TASK_READY,     EVENT_SCHEDULE,  sReady_Schedule,   TASK_RUNNING},
		{TASK_RUNNING,   EVENT_YIELD,     sRunning_Yield,    TASK_READY},
		{TASK_RUNNING,   EVENT_BLOCK,     sRunning_Block,    TASK_BLOCKED},
		{TASK_RUNNING,   EVENT_SUSPEND,   sRunning_Suspend,  TASK_SUSPENDED},
		{TASK_BLOCKED,   EVENT_UNBLOCK,   sBlocked_Unblock,  TASK_READY},
		{TASK_SUSPENDED, EVENT_RESUME,    sSuspended_Resume, TASK_READY}
};

void Kernel_StateM_Transaction(uint32_t task_id, KernelTaskEvent_t event) {
	KernelTaskState_t current_state = Kernel_Task_Get_State(task_id);
	for (uint32_t i=0; i < NUM_TRANSACTIONS; i++) {
		if (current_state == table[i].current_state && event == table[i].event) {
			__set_BASEPRI(0x10);
			Kernel_Task_Set_State(task_id, table[i].next_state);
			table[i].action(task_id);
			__set_BASEPRI(0x0);
			break;
		}
	}
}

void sReady_Schedule(uint32_t task_id) {
	Kernel_Task_Set_Current_Task_Id(task_id);
}

void sRunning_Yield(uint32_t task_id) {
	if (task_id != IDLE_TASK_ID) {
		Kernel_TaskQ_Enqueue(TASK_READY, task_id);
	}
	Port_trigger_pendsv();
}
void sRunning_Block(uint32_t task_id) {
	Kernel_TaskQ_Enqueue(TASK_BLOCKED, task_id);
	Port_trigger_pendsv();
}
void sBlocked_Unblock(uint32_t task_id) {
	Kernel_TaskQ_Enqueue(TASK_READY, task_id);
}

void sRunning_Suspend(uint32_t task_id) {

}

void sSuspended_Resume(uint32_t task_id) {

}
