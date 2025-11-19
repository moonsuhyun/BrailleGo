#ifndef KERNEL_TASK_H_
#define KERNEL_TASK_H_

#include "types.h"
#include "SysCall.h"

class Task {
private:
	uint32_t* m_stack_pointer;
	uint8_t* m_stack_base;
	int32_t  m_time_slice;
	uint32_t m_wake_time;
	KernelTaskState_t m_state;
	uint32_t m_id;

	// priority
	uint32_t m_priority;
	uint32_t m_base_priority;
	Task* m_next_task{nullptr};

	// signal
	// sigset_t  m_sig_pending;
	// sigset_t  m_sig_mask;
	// int       m_in_sig;
	// sigset_t  m_saved_mask;
	// struct sigaction m_sigv[32];

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
	void initSchedule(void);
	void initYield(void);
	void runningMutex(void);
public:
	Task() {}
	void Init(uint32_t* stack_pointer, uint8_t* stack_base, uint32_t id);
	void SetProgramCounter(uint32_t wrapper, uint32_t pc, void* arg);
//	void SetStackFrame(const TaskStackFrame_t& frame);
	void SetNextState(KernelTaskEvent_t event);
	void SetDelayTime(uint32_t ms);
	uint32_t GetWakeTime(void) const;
	bool IsDelayTimeOver(void) const;
	void InitIdleTask(uint32_t wrapper, uint32_t pc);
	uint32_t GetId(void) const;
	uint32_t GetPriority(void) const;
	Task* GetNextTask(void) const;
	KernelTaskState_t GetState(void) const;
	void SetNextTask(Task* task);
	void SetPriority(uint32_t priority);
	void InheritProirity(uint32_t to);
	void RestorePriority(void);
	void DecreaseTimeSlice();
	// void PendSignal(int32_t sig);
	// void SetSigAction(int32_t sig, struct sigaction action);
	// struct sigaction GetSigAction(int32_t sig);
	// sigset_t GetSigMask(void);
	// void SetSigBlock(sigset_t set);
	// void SetSigUnblock(sigset_t set);
	// void SetSigMask(sigset_t set);
	// sigset_t GetSigPending();
	// void SigReturn();
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
//void TaskManager_Scheduler(void);
//
//uint32_t Kernel_Task_Get_Idle_Task_Id(void);
//uint32_t TaskManager_Get_Current_Task_Id(void);
//void Kernel_Task_Set_Current_Task_Id(uint32_t task_id);
//
//uint32_t Kernel_Task_Get_State(uint32_t task_id);
//void Kernel_Task_Set_State(uint32_t task_id, KernelTaskState_t state);
//
//KernelTcb_t* Kernel_Task_Get_Current_Tcb(void);
//
//void TaskManager_SysTick_Callback(void);

#endif // KERNEL_TASK_H_
