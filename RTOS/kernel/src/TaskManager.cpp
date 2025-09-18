/*
 * TaskManager.cpp
 *
 *  Created on: Jul 8, 2025
 *      Author: moon
 */

#include <TaskManager.hpp>
#include "taskq.h"
#include "PortTask.h"
#include "stdio.h"
#include "BspMemoryMap.h"
#include "BspSysTick.h"

TaskManager::TaskManager() {
	m_allocated_task_count = 0;
	m_running_task_id = 0;
}

TaskManager::~TaskManager() {
	// TODO Auto-generated destructor stub
}

const Task* TaskManager::GetRunningTask(void) {
	return &m_task_list[m_running_task_id];
}

void TaskManager::SysTickCallback(void) {
	if (!Kernel_TaskQ_Is_Empty(TASK_BLOCKED_DELAY)) {
		TaskQIterator_t iter;
		uint32_t task_id;
		if (Kernel_TaskQ_Iterator_Init(&iter, TASK_BLOCKED_DELAY, &task_id)){
			while (Kernel_TaskQ_Iterator_Get(&iter)) {
				if (m_task_list[task_id].IsDelayTimeOver()) {
					Kernel_TaskQ_Remove(TASK_BLOCKED_DELAY, task_id);
					m_task_list[task_id].SetNextState(EVENT_UNBLOCK);
				}
			}
		}
	}
}

void TaskManager::SetRunningTaskID(uint32_t id) {
	m_running_task_id = id;
}

uint32_t TaskManager::GetInitTaskID(void) {
	return m_init_task_id;
}

void TaskManager::Init(void) {
	Kernel_TaskQ_Init();
	for (uint32_t i = 0; i < MAX_TASK_NUM; i++) {
		uint32_t* stack_pointer = (uint32_t*)(TASK_STACK_TOP - i * TASK_STACK_SIZE);
		uint8_t* stack_base = (uint8_t*)(TASK_STACK_TOP - (i + 1) * TASK_STACK_SIZE);
		m_task_list[i].Init(stack_pointer, stack_base, i);
	}
//	m_idle_task_id = TaskCreate(&TaskManager::sIdleTask);
	m_init_task_id = m_allocated_task_count++;
	m_task_list[m_init_task_id].InitIdleTask((uint32_t)&TaskManager::sInitTask);
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
	Kernel_TaskQ_Dequeue(TASK_READY, &task_id);
	return task_id;
}

void TaskManager::Start(void) {
	m_task_list[m_init_task_id].SetNextState(EVENT_SCHEDULE);
	Port_Task_Start();
}

uint32_t TaskManager::TaskCreate(KernelTaskFunc_t start_func) {
	uint32_t task_id = m_allocated_task_count++;

	if (m_allocated_task_count > MAX_TASK_NUM) return NOT_ENOUGH_TASK_NUM;

	Task& new_task = m_task_list[task_id];
	new_task.SetProgramCounter((uint32_t)start_func);
	new_task.SetNextState(EVENT_CREATE);

	return task_id;
}

void TaskManager::TaskYield(void) {
	m_task_list[m_running_task_id].SetNextState(EVENT_YIELD);
}

void TaskManager::TaskDelay(uint32_t ms) {
	m_task_list[m_running_task_id].SetDelayTime(ms);
	m_task_list[m_running_task_id].SetNextState(EVENT_DELAY);
}

void TaskManager::TaskTerminate(void) {
	m_task_list[m_running_task_id].SetNextState(EVENT_TERMINATE);
}

void TaskManager::Scheduler(void) {
	uint32_t task_id;
	if (Kernel_TaskQ_Is_Empty(TASK_READY)) {
		task_id = m_init_task_id;
		printf("[Tick %u] Idle task(id=%u) scheduled\r\n", BSP_Get_Tick(), task_id);
	} else {
		task_id = schedulerRoundRobinAlgorithm();
	}
    m_task_list[task_id].SetNextState(EVENT_SCHEDULE);
}


void TaskManager::sInitTask(void) {
	TaskManager& task_manager = TaskManager::sGetInstance();
	task_manager.TaskYield();
	while (1) {
		if (!Kernel_TaskQ_Is_Empty(TASK_READY)) task_manager.TaskYield();
		//Port_Core_Wait_For_Interrupt();
	}
}


const Task* Kernel_Task_Get_Current_Task(void) {
	TaskManager& task_manager = TaskManager::sGetInstance();
	return task_manager.GetRunningTask();

}

void Kernel_Task_SysTick_Callback(void) {
	TaskManager& task_manager = TaskManager::sGetInstance();
	task_manager.SysTickCallback();
}

void Kernel_Task_Scheduler(void) {
	TaskManager& task_manager = TaskManager::sGetInstance();
	task_manager.Scheduler();
}
