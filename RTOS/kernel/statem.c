/*
 * statem.c
 *
 *  Created on: May 27, 2025
 *      Author: moon
 */

#include "statem.h"
#include "stm32f1xx.h"

extern KernelTcb_t* gCurrent_tcb;

KernelEventActionTable_t table[NUM_TRANSACTIONS] = {
//		|CUR_STATE----|EVENT---------|ACTION------------------------|NEXT_STATE----|
		{TASK_READY,   EVENT_RESUME,  Kernel_statem_ready_resume,    TASK_RUNNING},
		{TASK_RUNNING, EVENT_YIELD,   Kernel_statem_running_yield,   TASK_READY},
		{TASK_RUNNING, EVENT_BLOCK,   Kernel_statem_running_block,   TASK_BLOCKED},
		{TASK_BLOCKED, EVENT_UNBLOCK, Kernel_statem_blocked_unblock, TASK_READY}
};

void Kernel_statem_transaction(uint32_t task_id, KernelTaskEvent_t event) {
	KernelTaskState_t current_state = Kernel_task_get_state(task_id);
	for (uint32_t i=0; i < NUM_TRANSACTIONS; i++) {
		if (current_state == table[i].current_state && event == table[i].event) {
			__set_BASEPRI(0x10);
			Kernel_task_set_state(task_id, table[i].next_state);
			table[i].action(task_id);
			__set_BASEPRI(0x0);
			break;
		}
	}
}

//void Kernel_statem_transaction(KernelTcb_t* tcb, KernelTaskEvent_t event) {
//	KernelTaskState_t current_state = tcb.state;
//	for (uint32_t i; i < NUM_TRANSATION; i++) {
//		if (current_state == table[i].current_state && event == table[i].event) {
//			gCurrent_tcb.state = table[i].next_state;
//			table[i].action();
//			break;
//		}
//	}
//}

void Kernel_statem_ready_resume(uint32_t task_id) {
	Kernel_task_set_current_task_id(task_id);
}

void Kernel_statem_running_yield(uint32_t task_id) {
	Kernel_taskq_enqueue(TASK_READY, task_id);
	Port_trigger_pendsv();
}
void Kernel_statem_running_block(uint32_t task_id) {

}
void Kernel_statem_blocked_unblock(uint32_t task_id) {

}
