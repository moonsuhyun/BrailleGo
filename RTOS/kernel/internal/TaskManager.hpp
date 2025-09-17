/*
 * TaskManager.hpp
 *
 *  Created on: Jul 9, 2025
 *      Author: moon
 */

#ifndef KERNEL_INTERNAL_TASKMANAGER_HPP_
#define KERNEL_INTERNAL_TASKMANAGER_HPP_

#include <Task.hpp>

typedef void (*KernelTaskFunc_t)(void);

class TaskManager {
private:
	Task m_task_list[MAX_TASK_NUM];
	uint32_t m_allocated_task_count;
	uint32_t m_init_task_id;
	uint32_t m_running_task_id;
	TaskManager();
	uint32_t schedulerRoundRobinAlgorithm(void);
//	static TaskManager s_instance;
	static void sInitTask(void);
public:
	virtual ~TaskManager();
	void Init(void);
	void Start(void);
	uint32_t TaskCreate(KernelTaskFunc_t start_func);
	void TaskYield(void);
	void TaskDelay(uint32_t ms);
	void TaskTerminate(void);
	void Scheduler(void);
	const Task* GetRunningTask(void);
	void SysTickCallback(void);
	void SetRunningTaskID(uint32_t id);
	uint32_t GetInitTaskID(void);
	static TaskManager& sGetInstance(void);
//	static const Task* sGetCurrentTask(void);
//	static void sSysTickCallback(void);
};

#ifdef __cplusplus
extern "C" {
#endif

const Task* Kernel_Task_Get_Current_Task(void);
void Kernel_Task_SysTick_Callback(void);
void Kernel_Task_Scheduler(void);

#ifdef __cplusplus
}
#endif


#endif /* KERNEL_INTERNAL_TASKMANAGER_HPP_ */
