#ifndef KERNEL_TASK_H_
#define KERNEL_TASK_H_

#include "types.h"

class Task {
private:
	uint32_t* m_stack_pointer;
	uint8_t* m_stack_base;
	uint32_t m_time_slice;
	uint32_t m_wake_time;
	KernelTaskState_t m_state;
	uint32_t m_id;
	typedef struct KernelEventActionTable {
		KernelTaskState_t current_state;
		KernelTaskEvent_t event;
		void (Task::*action)(void);
		KernelTaskState_t next_state;
	} KernelEventActionTable_t;
	void readySchedule(void);
	void runningYield(void);
	void runningDelay(void);
	void blockedUnblock(void);
	void runningSuspend(void);
	void suspendResume(void);
	void runningTerminate(void);
	void terminatedCreate(void);
	void idleSchedule(void);
	void idleYield(void);
public:
	Task() {}
	void Init(uint32_t* stack_pointer, uint8_t* stack_base, uint32_t id);
	void SetProgramCounter(uint32_t wrapper, uint32_t pc, void* arg);
//	void SetStackFrame(const TaskStackFrame_t& frame);
	void SetNextState(KernelTaskEvent_t event);
	void SetDelayTime(uint32_t ms);
	uint32_t GetWakeTime(void);
	bool IsDelayTimeOver(void);
	void InitIdleTask(uint32_t wrapper, uint32_t pc);
};

//typedef void (*KernelTaskFunc_t)(void);
//
//void Kernel_Task_Init(void);
//void Kernel_Task_Start(void);
//uint32_t Kernel_Task_Create(KernelTaskFunc_t start_func);
//void Kernel_Task_Yield(uint32_t task_id);
//void Kernel_Task_Delay(uint32_t task_id, uint32_t ms);
//void Kernel_Task_Terminate(uint32_t task_id);
//
//void Kernel_Task_Scheduler(void);
//
//uint32_t Kernel_Task_Get_Idle_Task_Id(void);
//uint32_t Kernel_Task_Get_Current_Task_Id(void);
//void Kernel_Task_Set_Current_Task_Id(uint32_t task_id);
//
//uint32_t Kernel_Task_Get_State(uint32_t task_id);
//void Kernel_Task_Set_State(uint32_t task_id, KernelTaskState_t state);
//
//KernelTcb_t* Kernel_Task_Get_Current_Tcb(void);
//
//void Kernel_Task_SysTick_Callback(void);

#endif // KERNEL_TASK_H_
