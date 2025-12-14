/*
 * TaskManager.cpp
 *
 *  Created on: Jul 8, 2025
 *      Author: moon
 */

#include <TaskManager.hpp>
#include "TaskManager.h"

#include "TaskQ.h"
#include "PortTask.h"
#include <cstdio>
#include "BspMemoryMap.h"
#include "BspSysTick.h"
#include "Types.h"

TaskManager::~TaskManager() {
	// TODO Auto-generated destructor stub
}

Task* TaskManager::GetRunningTask(void) {
	return &m_task_list[m_running_task_id];
}

uint32_t TaskManager::GetRunningTaskId(void) const
{
	return m_running_task_id;
}

uint32_t TaskManager::GetWakeTimeById(uint32_t id)
{
	return m_task_list[id].GetWakeTime();
}

void TaskManager::SysTickCallback(void) {
	while (!Kernel_TaskQ_Is_Empty(TASK_BLOCKED_DELAY))
	{
		uint32_t task_id;
		Kernel_TaskQ_Get_Front(TASK_BLOCKED_DELAY, &task_id);
		if (!m_task_list[task_id].IsDelayTimeOver()) break;
		Kernel_TaskQ_Dequeue(TASK_BLOCKED_DELAY, &task_id);
		m_task_list[task_id].SetNextState(EVENT_UNBLOCK);
	}
	m_task_list[m_running_task_id].DecreaseTimeSlice();
}

void TaskManager::SetRunningTaskID(uint32_t id) {
	m_running_task_id = id;
}

void TaskManager::Init(KernelTaskFunc_t init_task) {
	Kernel_TaskQ_Init();

	for (uint32_t i = 0; i < MAX_TASK_NUM; i++) {
		uint32_t* stack_pointer = (uint32_t*)(TASK_STACK_TOP - i * TASK_STACK_SIZE);
		uint8_t* stack_base = (uint8_t*)(TASK_STACK_TOP - (i + 1) * TASK_STACK_SIZE);
		m_task_list[i].Init(stack_pointer, stack_base, i);
	}

	initTaskCreate(init_task, nullptr);
}

TaskManager& TaskManager::sGetInstance() {
	static TaskManager instance;
	return instance;
}

uint32_t TaskManager::schedulerRoundRobinAlgorithm(void) {
	uint32_t task_id;
	Kernel_TaskQ_Get_Front(TASK_READY, &task_id);
	return task_id;
}

uint32_t TaskManager::initTaskCreate(KernelTaskFunc_t start_func, void* arg)
{
	uint32_t task_id = m_allocated_task_count++;
	Task& new_task = m_task_list[task_id];
	new_task.SetPriority(0);
	new_task.SetProgramCounter((uint32_t)&TaskManager::sTaskWrapper, (uint32_t)start_func, arg);
	new_task.SetNextState(EVENT_INIT);

	return task_id;
}

uint32_t TaskManager::TaskCreate(KernelTaskFunc_t start_func, void* arg, uint32_t priority)
{
	uint32_t task_id = m_allocated_task_count++;

	if (m_allocated_task_count > MAX_TASK_NUM) return NOT_ENOUGH_TASK_NUM;

	Task& new_task = m_task_list[task_id];
	new_task.SetPriority(priority);
	new_task.SetProgramCounter((uint32_t)&TaskManager::sTaskWrapper, (uint32_t)start_func, arg);
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
	Task* next_task = m_ready_queue.Pop();

	if (!next_task)
	{
		next_task = &m_task_list.at(INIT_TASK_ID);
		// uint32_t tick = BSP_Get_Tick();
		// if (tick % 1000 == 0)
		// {
		// 	printf("[Tick %u] Init task scheduled\r\n", BSP_Get_Tick());
		// }
	}

   next_task->SetNextState(EVENT_SCHEDULE);
}

void TaskManager::MutexWait(uint32_t owner_id)
{
	// Priority Inheritance
	uint32_t current_priority = GetRunningTask()->GetPriority();
	uint32_t owner_priority = m_task_list.at(owner_id).GetPriority();
	if (current_priority < owner_priority)
	{
		if (m_ready_queue.Remove(&m_task_list.at(owner_id)))
		{
			m_ready_queue.Push(&m_task_list.at(owner_id), current_priority);
		}
		m_task_list.at(owner_id).InheritPriority(current_priority);
	}
	m_task_list.at(m_running_task_id).SetNextState(EVENT_WAIT);
}

void TaskManager::MutexWake(Task* next)
{
	m_task_list.at(m_running_task_id).RestorePriority();
	next->SetNextState(EVENT_UNBLOCK);
}

void TaskManager::SemWait(void)
{
	m_task_list.at(m_running_task_id).SetNextState(EVENT_WAIT);
}

void TaskManager::SemWake(Task* next)
{
	next->SetNextState(EVENT_UNBLOCK);
}

void TaskManager::PushToReadyById(uint32_t id)
{
	m_ready_queue.Push(&m_task_list.at(id));
}

// int32_t TaskManager::PendSignalById(uint32_t pid, int32_t sig)
// {
// 	if (pid >= MAX_TASK_NUM || m_task_list.at(pid).GetState() == TASK_TERMINATED) return -ESRCH;
// 	m_task_list.at(pid).PendSignal(sig);
// 	return 0;
// }


void TaskManager::sIdleTask(void) {
	TaskManager& task_manager = TaskManager::sGetInstance();
	task_manager.TaskYield();
	while (1) {
		if (!Kernel_TaskQ_Is_Empty(TASK_READY)) task_manager.TaskYield();
		//Port_Core_Wait_For_Interrupt();
	}
}

__attribute ((noreturn))
void TaskManager::sTaskWrapper(void (*task_func)(void*), void* arg)
{
	task_func(arg);
	TaskManager::sGetInstance().TaskTerminate();

	__builtin_unreachable();
}


Task* TaskManager_Get_Current_Task(void) {
	TaskManager& task_manager = TaskManager::sGetInstance();
	return task_manager.GetRunningTask();
}

int32_t TaskManager_Get_Current_Task_Id()
{
	TaskManager& task_manager = TaskManager::sGetInstance();
	return task_manager.GetRunningTaskId();
}


void TaskManager_SysTick_Callback(void) {
	TaskManager& task_manager = TaskManager::sGetInstance();
	task_manager.SysTickCallback();
}

void TaskManager_Scheduler(void) {
	TaskManager& task_manager = TaskManager::sGetInstance();
	task_manager.Scheduler();
}

uint32_t Kernel_Task_Get_Wake_Time_By_Id(uint32_t id)
{
	return TaskManager::sGetInstance().GetWakeTimeById(id);
}


// System Call Functions
// SVC 핸들러에서 호출하는 함수
void TaskManager_Init(KernelTaskFunc_t init_task)
{
	TaskManager& task_manager = TaskManager::sGetInstance();
	task_manager.Init(init_task);
}

int32_t TaskManager_Create(KernelTaskFunc_t start_func, void* arg, uint32_t priority)
{
	TaskManager& task_manager = TaskManager::sGetInstance();
	return task_manager.TaskCreate(start_func, arg, priority);
}

int32_t TaskManager_Yield()
{
	TaskManager& task_manager = TaskManager::sGetInstance();
	task_manager.TaskYield();
	return 0;
}

int32_t TaskManager_Delay(uint32_t ms)
{
	TaskManager& task_manager = TaskManager::sGetInstance();
	task_manager.TaskDelay(ms);

	return 0;
}

int32_t TaskManager_Terminate()
{
	TaskManager& task_manager = TaskManager::sGetInstance();
	task_manager.TaskTerminate();

	return 0;
}
