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

	// 동기화 객체(Mutex, Semaphore, WaitList)가 호출
	uint32_t GetPriorityById(uint32_t id) const;
	void     Wake(uint32_t id);
	void     MutexWait(uint32_t owner_id);
	void     MutexWake(Task* next);
	void     SemWait(void);

	// 조회
	Task*    GetRunningTask(void);
	uint32_t GetRunningTaskId(void) const;
	uint32_t GetWakeTimeById(uint32_t id);
};

#endif /* KERNEL_INTERNAL_TASKMANAGER_HPP_ */
