/*
 * TaskManager.cpp
 *
 *  Created on: Jul 8, 2025
 *      Author: moon
 */

#include <TaskManager.hpp>
#include "TaskQ.h"
#include "PortTask.h"
#include "stdio.h"
#include "BspMemoryMap.h"
#include "BspSysTick.h"
#include "types.h"

Task TaskManager::s_task_list[MAX_TASK_NUM];

TaskManager::TaskManager() {
	m_allocated_task_count = 0;
	m_running_task_id = 0;
}

TaskManager::~TaskManager() {
	// TODO Auto-generated destructor stub
}

const Task* TaskManager::GetRunningTask(void) {
	return &s_task_list[m_running_task_id];
}

uint32_t TaskManager::GetRunningTaskId(void)
{
	return m_running_task_id;
}

uint32_t TaskManager::GetWakeTimeById(uint32_t id)
{
	return s_task_list[id].GetWakeTime();
}

void TaskManager::SysTickCallback(void) {
	while (!Kernel_TaskQ_Is_Empty(TASK_BLOCKED_DELAY))
	{
		uint32_t task_id;
		Kernel_TaskQ_Get_Front(TASK_BLOCKED_DELAY, &task_id);
		if (!s_task_list[task_id].IsDelayTimeOver()) break;
		Kernel_TaskQ_Dequeue(TASK_BLOCKED_DELAY, &task_id);
		s_task_list[task_id].SetNextState(EVENT_UNBLOCK);
	}
}

void TaskManager::SetRunningTaskID(uint32_t id) {
	m_running_task_id = id;
}

// uint32_t TaskManager::GetInitTaskID(void) {
// 	return m_init_task_id;
// }

void TaskManager::Init(void) {
	Kernel_TaskQ_Init();
	for (uint32_t i = 0; i < MAX_TASK_NUM; i++) {
		uint32_t* stack_pointer = (uint32_t*)(TASK_STACK_TOP - i * TASK_STACK_SIZE);
		uint8_t* stack_base = (uint8_t*)(TASK_STACK_TOP - (i + 1) * TASK_STACK_SIZE);
		s_task_list[i].Init(stack_pointer, stack_base, i);
	}
//	m_idle_task_id = TaskCreate(&TaskManager::sIdleTask);
	s_task_list[m_allocated_task_count++].InitIdleTask((uint32_t)&TaskManager::sTaskWrapper, (uint32_t)&TaskManager::sIdleTask);
}

TaskManager& TaskManager::sGetInstance() {
//	if (s_instance == nullptr) {
//		s_instance = TaskManager();
//	}
//	return s_instance;
	static TaskManager instance;
	return instance;
}

uint32_t TaskManager::schedulerRoundRobinAlgorithm(void) {
	uint32_t task_id;
	Kernel_TaskQ_Get_Front(TASK_READY, &task_id);
	return task_id;
}

void TaskManager::Start(void) {
	s_task_list[IDLE_TASK_ID].SetNextState(EVENT_SCHEDULE);
	Port_Task_Start();
}

uint32_t TaskManager::TaskCreate(KernelTaskFunc_t start_func, void* arg) {
	uint32_t task_id = m_allocated_task_count++;

	if (m_allocated_task_count > MAX_TASK_NUM) return NOT_ENOUGH_TASK_NUM;

	Task& new_task = s_task_list[task_id];
	new_task.SetProgramCounter((uint32_t)&TaskManager::sTaskWrapper, (uint32_t)start_func, arg);
	new_task.SetNextState(EVENT_CREATE);

	return task_id;
}

void TaskManager::TaskYield(void) {
	s_task_list[m_running_task_id].SetNextState(EVENT_YIELD);
}

void TaskManager::TaskDelay(uint32_t ms) {
	s_task_list[m_running_task_id].SetDelayTime(ms);
	s_task_list[m_running_task_id].SetNextState(EVENT_DELAY);
}

void TaskManager::TaskTerminate(void) {
	s_task_list[m_running_task_id].SetNextState(EVENT_TERMINATE);
}

void TaskManager::Scheduler(void) {
	uint32_t task_id;
	if (Kernel_TaskQ_Is_Empty(TASK_READY)) {
		task_id = IDLE_TASK_ID;
		printf("[Tick %u] Idle task(id=%u) scheduled\r\n", BSP_Get_Tick(), task_id);
	} else {
		task_id = schedulerRoundRobinAlgorithm();
	}
    s_task_list[task_id].SetNextState(EVENT_SCHEDULE);
}


void TaskManager::sIdleTask(void) {
	TaskManager& task_manager = TaskManager::sGetInstance();
	task_manager.TaskYield();
	while (1) {
		if (!Kernel_TaskQ_Is_Empty(TASK_READY)) task_manager.TaskYield();
		//Port_Core_Wait_For_Interrupt();
	}
}

void TaskManager::sTaskWrapper(void (*task_func)(void*), void* arg)
{
	task_func(arg);
	TaskManager::sGetInstance().TaskTerminate();
}


const Task* Kernel_Task_Get_Current_Task(void) {
	TaskManager& task_manager = TaskManager::sGetInstance();
	return task_manager.GetRunningTask();
}

// void Kernel_Task_Wrapper(void (*task_func)(void*), void* arg)
// {
// 	task_func(arg);
// 	TaskManager::sGetInstance().TaskTerminate();
// }

int32_t Kernel_Task_Get_Current_Task_Id()
{
	TaskManager& task_manager = TaskManager::sGetInstance();
	return task_manager.GetRunningTaskId();
}


void Kernel_Task_SysTick_Callback(void) {
	TaskManager& task_manager = TaskManager::sGetInstance();
	task_manager.SysTickCallback();
}

void Kernel_Task_Scheduler(void) {
	TaskManager& task_manager = TaskManager::sGetInstance();
	task_manager.Scheduler();
}

uint32_t Kernel_Task_Get_Wake_Time_By_Id(uint32_t id)
{
	return TaskManager::sGetInstance().GetWakeTimeById(id);
}

