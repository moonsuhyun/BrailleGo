#ifndef KERNEL_INTERNAL_TASK_H_
#define KERNEL_INTERNAL_TASK_H_

#include "Types.h"

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
	void runningWait(void);
public:
	Task() {}
	void Init(uint32_t* stack_pointer, uint8_t* stack_base, uint32_t id);
	void SetProgramCounter(uint32_t wrapper, uint32_t pc, void* arg);
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
	void InheritPriority(uint32_t to);
	void RestorePriority(void);
	void DecreaseTimeSlice();
};

#endif // KERNEL_INTERNAL_TASK_H_
