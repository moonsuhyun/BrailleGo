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

uint32_t TaskManager::GetRunningTaskId(void) const
{
    return m_running_task_id;
}

uint32_t TaskManager::GetWakeTimeById(uint32_t const id) const
{
    return m_task_list[id].GetWakeTime();
}

void TaskManager::SysTickCallback(void)
{
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

void TaskManager::SetRunningTaskID(uint32_t const id)
{
    m_running_task_id = id;
}

void TaskManager::Init(KernelTaskFunc_t const init_task)
{
    Kernel_TaskQ_Init();

    for (uint32_t i = 0; i < MAX_TASK_NUM; i++)
    {
        auto* stack_pointer = reinterpret_cast<uint32_t*>((TASK_STACK_TOP - i * TASK_STACK_SIZE));
        auto* stack_base    = reinterpret_cast<uint8_t*>((TASK_STACK_TOP - (i + 1) * TASK_STACK_SIZE));
        m_task_list[i].Init(stack_pointer, stack_base, i);
    }

    initTaskCreate(init_task, nullptr);
}

TaskManager& TaskManager::sGetInstance()
{
    static TaskManager instance;
    return instance;
}

uint32_t TaskManager::initTaskCreate(KernelTaskFunc_t start_func, void* arg)
{
    uint32_t const task_id = m_allocated_task_count++;
    Task& new_task = m_task_list[task_id];
    new_task.SetPriority(0);
    new_task.SetProgramCounter(reinterpret_cast<uint32_t>(&TaskManager::sTaskWrapper), reinterpret_cast<uint32_t>(start_func), arg);
    new_task.SetNextState(EVENT_INIT);

    return task_id;
}

int32_t TaskManager::TaskCreate(KernelTaskFunc_t start_func, void* arg, uint32_t const priority)
{
    uint32_t const task_id = m_allocated_task_count++;

    if (m_allocated_task_count > MAX_TASK_NUM) return NOT_ENOUGH_TASK_NUM;

    Task& new_task = m_task_list[task_id];
    new_task.SetPriority(priority);
    new_task.SetProgramCounter(reinterpret_cast<uint32_t>(&TaskManager::sTaskWrapper), reinterpret_cast<uint32_t>(start_func), arg);
    new_task.SetNextState(EVENT_CREATE);

    return static_cast<int32_t>(task_id);
}

void TaskManager::TaskYield(void)
{
    m_task_list[m_running_task_id].SetNextState(EVENT_YIELD);
}

void TaskManager::TaskDelay(uint32_t const ms)
{
    m_task_list[m_running_task_id].SetDelayTime(ms);
    m_task_list[m_running_task_id].SetNextState(EVENT_DELAY);
}

void TaskManager::TaskTerminate(void)
{
    m_task_list[m_running_task_id].SetNextState(EVENT_TERMINATE);
}

void TaskManager::Scheduler(void)
{
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

void TaskManager::TaskWait(void)
{
    m_task_list.at(m_running_task_id).SetNextState(EVENT_WAIT);
}

void TaskManager::TaskWake(uint32_t const id)
{
    m_task_list.at(id).SetNextState(EVENT_UNBLOCK);
}

uint32_t TaskManager::GetPriorityById(uint32_t const id) const
{
    return m_task_list.at(id).GetPriority();
}

uint32_t TaskManager::GetBasePriorityById(uint32_t const id) const
{
    return m_task_list.at(id).GetBasePriority();
}

void TaskManager::SetPriorityById(uint32_t const id, uint32_t const priority)
{
    Task& task = m_task_list.at(id);

    if (task.GetPriority() != priority)
    {
        bool const in_ready_queue = (task.GetState() == TASK_READY);

        // ready queue에 없는 경우, 그냥 priority 값만 바꾸면 됨
        if (in_ready_queue)
        {
            m_ready_queue.Remove(&task); // 변경 전 priority의 버킷에서 제거
        }

        task.InheritPriority(priority);  // priority 변경

        if (in_ready_queue)
        {
            m_ready_queue.PushBack(&task); // 변경된 priority의 버킷에 추가
        }
    }
}

KernelTcb_t* TaskManager::GetRunningTcb(void)
{
    return m_task_list.at(m_running_task_id).GetTcb();
}

void TaskManager::PushToReadyById(uint32_t const id)
{
    m_ready_queue.PushBack(&m_task_list.at(id));
}

__attribute ((noreturn))
void TaskManager::sTaskWrapper(void (*task_func)(void*), void* arg)
{
    task_func(arg);
    TaskManager::sGetInstance().TaskTerminate();

    __builtin_unreachable();
}


KernelTcb_t* TaskManager_Get_Current_Task(void)
{
    TaskManager& task_manager = TaskManager::sGetInstance();
    return task_manager.GetRunningTcb();
}

int32_t TaskManager_Get_Current_Task_Id()
{
    TaskManager const& task_manager = TaskManager::sGetInstance();
    return static_cast<int32_t>(task_manager.GetRunningTaskId());
}


void TaskManager_SysTick_Callback(void)
{
    TaskManager& task_manager = TaskManager::sGetInstance();
    task_manager.SysTickCallback();
}

void TaskManager_Scheduler(void)
{
    TaskManager& task_manager = TaskManager::sGetInstance();
    task_manager.Scheduler();
}

uint32_t Kernel_Task_Get_Wake_Time_By_Id(uint32_t const id)
{
    return TaskManager::sGetInstance().GetWakeTimeById(id);
}


// System Call Functions
// SVC 핸들러에서 호출하는 함수
void TaskManager_Init(KernelTaskFunc_t const init_task)
{
    TaskManager& task_manager = TaskManager::sGetInstance();
    task_manager.Init(init_task);
}

int32_t TaskManager_Create(KernelTaskFunc_t const start_func, void* arg, uint32_t const priority)
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

int32_t TaskManager_Delay(uint32_t const ms)
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
