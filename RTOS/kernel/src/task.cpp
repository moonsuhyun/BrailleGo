
#include <Task.hpp>
#include "PortTask.h"
#include "stdio.h"
#include "memory.h"
#include "BspMemoryMap.h"
#include "taskq.h"
//#include "statem.h"
#include "BspSysTick.h"
#include "PortCore.h"
#include "TaskManager.hpp"

void Task::Init(uint32_t* stack_pointer, uint8_t* stack_base, uint32_t id) {
	m_stack_pointer = stack_pointer - sizeof(TaskStackFrame_t) / 4;
	m_stack_base = stack_base;
	m_id = id;
	memset(m_stack_base, 0, TASK_STACK_SIZE);
	*((int32_t*)m_stack_base) = STACK_CANARY_VALUE;
	m_wake_time = 0;
	m_state = TASK_TERMINATED;
}

void Task::SetProgramCounter(uint32_t wrapper, uint32_t pc, void* arg) {
    TaskStackFrame_t* task_frame = (TaskStackFrame_t*)m_stack_pointer;
    Port_Task_Create(task_frame, wrapper, pc, arg);
}

//void Task::SetStackFrame(const TaskStackFrame_t& frame) {
//	TaskStackFrame_t* task_frame = (TaskStackFrame_t*)m_stack_pointer;
//	*task_frame = frame;
//}

void Task::SetNextState(KernelTaskEvent_t event) {
	KernelEventActionTable_t table[] = {
	//		|CUR_STATE----------|EVENT-----------|ACTION------------------|NEXT_STATE---------|
			{TASK_READY,     	 EVENT_SCHEDULE,  &Task::readySchedule,    TASK_RUNNING},
			{TASK_RUNNING,   	 EVENT_YIELD,     &Task::runningYield,     TASK_READY},
			{TASK_RUNNING,    	 EVENT_DELAY,     &Task::runningDelay,     TASK_BLOCKED_DELAY},
			{TASK_RUNNING,   	 EVENT_SUSPEND,   &Task::runningSuspend,   TASK_SUSPENDED},
			{TASK_RUNNING,   	 EVENT_MUTEX,   &Task::runningMutex,   TASK_BLOCKED_MUTEX},
			{TASK_BLOCKED_MUTEX, EVENT_UNBLOCK,   &Task::blockedUnblock,   TASK_READY},
			{TASK_BLOCKED_DELAY, EVENT_UNBLOCK,   &Task::blockedUnblock,   TASK_READY},
			{TASK_SUSPENDED, 	 EVENT_RESUME,    &Task::suspendResume,    TASK_READY},
			{TASK_RUNNING,		 EVENT_TERMINATE, &Task::runningTerminate, TASK_TERMINATED},
			{TASK_TERMINATED,    EVENT_CREATE,    &Task::terminatedCreate, TASK_READY},
			{TASK_TERMINATED,    EVENT_INIT,    &Task::initSchedule, TASK_INIT_RUNNING},
			{TASK_INIT_READY, EVENT_SCHEDULE, &Task::initSchedule, TASK_INIT_RUNNING},
			{TASK_INIT_RUNNING, EVENT_YIELD, &Task::initYield, TASK_INIT_READY},
	};
	for (uint32_t i = 0; i < sizeof(table)/sizeof(table[0]); i++) {
		if (m_state == table[i].current_state && event == table[i].event) {
 			Port_Core_Disable_PendSV();
			m_state = table[i].next_state;
			if (table[i].action != nullptr) (this->*table[i].action)();
 			Port_Core_Enable_PendSV();
			break;
		}
	}
}

void Task::SetDelayTime(uint32_t ms) {
	m_wake_time = BSP_Get_Tick() + ms;
}

uint32_t Task::GetWakeTime(void) const
{
	return m_wake_time;
}

void Task::readySchedule(void) {
	TaskManager& task_manager = TaskManager::sGetInstance();
	m_time_slice = TIME_SLICE_QUANTUM;
	// Kernel_TaskQ_Dequeue(TASK_READY, nullptr);
	task_manager.SetRunningTaskID(m_id);
}

void Task::runningYield(void) {
	// Kernel_TaskQ_Enqueue(TASK_READY, m_id);
	TaskManager& task_manager = TaskManager::sGetInstance();
	task_manager.PushToReadyById(m_id);
	Port_Core_Trigger_PendSV();
}

void Task::runningDelay(void) {
	Kernel_TaskQ_Enqueue_Sorted_By_Wake_Time(TASK_BLOCKED_DELAY, m_id);
	Port_Core_Trigger_PendSV();
}

void Task::blockedUnblock(void) {
	// Kernel_TaskQ_Enqueue(TASK_READY, m_id);
	TaskManager& task_manager = TaskManager::sGetInstance();
	task_manager.PushToReadyById(m_id);
}

void Task::runningSuspend(void) {
}

void Task::suspendResume(void) {
}

void Task::runningTerminate(void) {
	// TaskManager& task_manager = TaskManager::sGetInstance();
	// task_manager.Scheduler();
	// Port_Task_Start();
	Port_Core_Trigger_PendSV();
}

void Task::terminatedCreate(void) {
	TaskManager& task_manager = TaskManager::sGetInstance();
	m_time_slice = TIME_SLICE_QUANTUM;
	task_manager.PushToReadyById(m_id);
}

void Task::initSchedule()
{
	TaskManager::sGetInstance().SetRunningTaskID(m_id);
}

void Task::initYield()
{
	Port_Core_Trigger_PendSV();
}

void Task::runningMutex()
{
	Port_Core_Trigger_PendSV();
}

bool Task::IsDelayTimeOver(void) const
{
	if (m_wake_time < BSP_Get_Tick()) return true;
	return false;
}

void Task::InitIdleTask(uint32_t wrapper, uint32_t pc) {
	TaskStackFrame_t* task_frame = (TaskStackFrame_t*)m_stack_pointer;
	Port_Task_Create(task_frame, wrapper, pc, NULL);
	m_state = TASK_INIT_READY;
}

uint32_t Task::GetId() const
{
	return m_id;
}

uint32_t Task::GetPriority() const
{
	return m_priority;
}

Task* Task::GetNextTask() const
{
	return m_next_task;
}

void Task::SetNextTask(Task* task)
{
	m_next_task = task;
}

void Task::SetPriority(uint32_t priority)
{
	m_priority = priority;
	m_base_priority = priority;
}

void Task::InheritProirity(uint32_t to)
{
	m_priority = to;
}

void Task::RestorePriority(void)
{
	m_priority = m_base_priority;
}

KernelTaskState_t Task::GetState() const
{
	return m_state;
}

void Task::DecreaseTimeSlice()
{
	if (this->m_state == TASK_RUNNING)
	{
		if (m_time_slice-- < 0)
		{
			this->SetNextState(EVENT_YIELD);
		}
	}
}
