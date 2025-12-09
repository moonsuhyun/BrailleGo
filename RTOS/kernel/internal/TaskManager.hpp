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

#include <sys/errno.h>

typedef void (*KernelTaskFunc_t)(void*);

class TaskManager {
private:
	// static Task s_task_list[];
	std::array<Task, MAX_TASK_NUM> m_task_list{};
	PriorityQueue m_ready_queue{};
	uint32_t m_allocated_task_count{0};
	// uint32_t m_init_task_id;
	uint32_t m_running_task_id{0};
	TaskManager() {}
	uint32_t schedulerRoundRobinAlgorithm(void);
	uint32_t initTaskCreate(KernelTaskFunc_t start_func, void* arg);
//	static TaskManager s_instance;
	static void sIdleTask(void);
	static void sTaskWrapper(void (*task_func)(void*), void* arg);
public:
	virtual ~TaskManager();
	// void Init(void);
	void Init(KernelTaskFunc_t init_task);
	uint32_t TaskCreate(KernelTaskFunc_t start_func, void* arg, uint32_t priority);
	void TaskYield(void);
	void TaskDelay(uint32_t ms);
	void TaskTerminate(void);
	void Scheduler(void);
	Task* GetRunningTask(void);
	uint32_t GetRunningTaskId(void) const;
	void SysTickCallback(void);
	void SetRunningTaskID(uint32_t id);
	uint32_t GetWakeTimeById(uint32_t id);
	void MutexWait(uint32_t owner_id);
	void MutexWake(Task* next);
	void SemWait(void);
	void SemWake(Task* next);
	void PushToReadyById(uint32_t id);
	int32_t PendSignalById(uint32_t pid, int32_t sig);
	static TaskManager& sGetInstance(void);

};

// #ifdef __cplusplus
// extern "C" {
// #endif
//
// // void Kernel_Task_Wrapper(void (*task_func)(void*), void* arg);
// const Task* TaskManager_Get_Current_Task(void);
// int32_t TaskManager_Get_Current_Task_Id(void);
// void TaskManager_SysTick_Callback(void);
// void TaskManager_Scheduler(void);
// uint32_t Kernel_Task_Get_Wake_Time_By_Id(uint32_t id);
//
// #ifdef __cplusplus
// }
// #endif


#endif /* KERNEL_INTERNAL_TASKMANAGER_HPP_ */
