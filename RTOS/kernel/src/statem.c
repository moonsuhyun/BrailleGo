/*
 * statem.c
 *
 *  Created on: May 27, 2025
 *      Author: moon
 */

#include "statem.h"

#include "PortCore.h"
#include "task.h"
#include "taskq.h"

static void sReady_Schedule(uint32_t task_id);
static void sRunning_Yield(uint32_t task_id);
static void sRunning_Delay(uint32_t task_id);
static void sBlocked_Unblock(uint32_t task_id);
static void sRunning_Suspend(uint32_t task_id);
static void sSuspended_Resume(uint32_t task_id);
static void sRunning_Terminate(uint32_t task_id);

KernelEventActionTable_t table[NUM_TRANSACTIONS] = {
//		|CUR_STATE----------|EVENT-----------|ACTION------------|NEXT_STATE----|
		{TASK_READY,     	 EVENT_SCHEDULE,  sReady_Schedule,   TASK_RUNNING},
		{TASK_RUNNING,   	 EVENT_YIELD,     sRunning_Yield,    TASK_READY},
		{TASK_RUNNING,    	 EVENT_DELAY,     sRunning_Delay,    TASK_BLOCKED_DELAY},
		{TASK_RUNNING,   	 EVENT_SUSPEND,   sRunning_Suspend,  TASK_SUSPENDED},
		{TASK_BLOCKED_DELAY, EVENT_UNBLOCK,   sBlocked_Unblock,  TASK_READY},
		{TASK_SUSPENDED, 	 EVENT_RESUME,    sSuspended_Resume, TASK_READY},
		{TASK_RUNNING,		 EVENT_TERMINATE, sRunning_Terminate,TASK_TERMINATED}
};

void Kernel_StateM_Transaction(uint32_t task_id, KernelTaskEvent_t event) {
	KernelTaskState_t current_state = Kernel_Task_Get_State(task_id);
	for (uint32_t i=0; i < NUM_TRANSACTIONS; i++) {
		if (current_state == table[i].current_state && event == table[i].event) {
			Port_Core_Disable_PendSV();
			Kernel_Task_Set_State(task_id, table[i].next_state);
			table[i].action(task_id);
			Port_Core_Enable_PendSV();
			break;
		}
	}
}

void sReady_Schedule(uint32_t task_id) {
	Kernel_Task_Set_Current_Task_Id(task_id);
}

void sRunning_Yield(uint32_t task_id) {
	if (task_id != Kernel_Task_Get_Idle_Task_Id()) {
		Kernel_TaskQ_Enqueue(TASK_READY, task_id);
	}
	Port_Core_Trigger_PendSV();
}
void sRunning_Delay(uint32_t task_id) {
	Kernel_TaskQ_Enqueue(TASK_BLOCKED_DELAY, task_id);
	Port_Core_Trigger_PendSV();
}
void sBlocked_Unblock(uint32_t task_id) {
	Kernel_TaskQ_Enqueue(TASK_READY, task_id);
}

void sRunning_Suspend(uint32_t task_id) {

}

void sSuspended_Resume(uint32_t task_id) {

}

void sRunning_Terminate(uint32_t task_id) {
	Kernel_Task_Scheduler();
	Port_Task_Start();
}
