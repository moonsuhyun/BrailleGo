/*
 * TaskManager.hpp
 *
 *  Created on: Jul 9, 2025
 *      Author: moon
 */

#ifndef KERNEL_INTERNAL_TASKMANAGER_HPP_
#define KERNEL_INTERNAL_TASKMANAGER_HPP_

#include "Task.hpp"
#include "PriorityQueue.hpp"

#include <array>

typedef void (*KernelTaskFunc_t)(void*);

class TaskManager {
private:
	std::array<Task, MAX_TASK_NUM> m_task_list{};
	PriorityQueue m_ready_queue{};
	uint32_t m_allocated_task_count{0};
	uint32_t m_running_task_id{0};

	TaskManager() {}
	uint32_t initTaskCreate(KernelTaskFunc_t start_func, void* arg);
	static void sTaskWrapper(void (*task_func)(void*), void* arg);

public:
	static TaskManager& sGetInstance(void);

	// 초기화, 태스크 생명주기 (SVC에서 호출)
	void    Init(KernelTaskFunc_t init_task);
	int32_t TaskCreate(KernelTaskFunc_t start_func, void* arg, uint32_t priority); // 태스크 ID, 실패 시 음수
	void    TaskYield(void);
	void    TaskDelay(uint32_t ms);
	void    TaskTerminate(void);

	// 스케줄링 (PendSV, SysTick, Task 상태 전이에서 호출)
	void Scheduler(void);
	void SysTickCallback(void);
	void SetRunningTaskID(uint32_t id);
	void PushToReadyById(uint32_t id);

	// 동기화 (Mutex, Semaphore, WaitList가 호출)
	uint32_t GetRunningTaskId(void) const;
	uint32_t GetPriorityById(uint32_t id) const;
	uint32_t GetBasePriorityById(uint32_t id) const;
	void     SetPriorityById(uint32_t id, uint32_t priority); // 우선순위 설정
	void     TaskWait(void); // 현재 태스크 -> BLOCKED_WAIT
	void     TaskWake(uint32_t id); // 블록된 태스크 -> READY

	// Port 접근용
	KernelTcb_t* GetRunningTcb(void);
	uint32_t     GetWakeTimeById(uint32_t id) const;
};

#endif /* KERNEL_INTERNAL_TASKMANAGER_HPP_ */
