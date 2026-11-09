#include "Task.hpp"

#include "PortTask.h"
#include "memory.h"
#include "BspMemoryMap.h"
#include "taskq.h"
#include "BspSysTick.h"
#include "PortCore.h"
#include "TaskManager.hpp"

void Task::Init(uint32_t* const stack_pointer, uint8_t* const stack_base, uint32_t const id)
{
    m_tcb.sp = stack_pointer - (sizeof(TaskStackFrame_t) / 4U);
    m_tcb.stack_base = stack_base;
    m_tcb.id = id;
    memset(m_tcb.stack_base, 0, TASK_STACK_SIZE);
    *reinterpret_cast<uint32_t*>(m_tcb.stack_base) = STACK_CANARY_VALUE;
    m_tcb.wake_time = 0;
    m_tcb.state = TASK_TERMINATED;
}

void Task::SetProgramCounter(uint32_t const wrapper, uint32_t const pc, void* const arg)
{
    auto* task_frame = reinterpret_cast<TaskStackFrame_t*>(m_tcb.sp);
    Port_Task_Create(task_frame, wrapper, pc, arg);
}

void Task::SetNextState(KernelTaskEvent_t const event)
{
    static KernelEventActionTable_t constexpr table[] = {
        {TASK_READY, EVENT_SCHEDULE, &Task::readySchedule, TASK_RUNNING},
        {TASK_RUNNING, EVENT_YIELD, &Task::runningYield, TASK_READY},
        {TASK_RUNNING, EVENT_DELAY, &Task::runningDelay, TASK_BLOCKED_DELAY},
        {TASK_RUNNING, EVENT_SUSPEND, &Task::runningSuspend, TASK_SUSPENDED},
        {TASK_RUNNING, EVENT_WAIT, &Task::runningWait, TASK_BLOCKED_WAIT},
        {TASK_BLOCKED_DELAY, EVENT_UNBLOCK, &Task::blockedUnblock, TASK_READY},
        {TASK_BLOCKED_WAIT, EVENT_UNBLOCK, &Task::blockedUnblock, TASK_READY},
        {TASK_SUSPENDED, EVENT_RESUME, &Task::suspendResume, TASK_READY},
        {TASK_RUNNING, EVENT_TERMINATE, &Task::runningTerminate, TASK_TERMINATED},
        {TASK_TERMINATED, EVENT_CREATE, &Task::terminatedCreate, TASK_READY},
        {TASK_TERMINATED, EVENT_INIT, &Task::initSchedule, TASK_INIT_RUNNING},
        {TASK_INIT_READY, EVENT_SCHEDULE, &Task::initSchedule, TASK_INIT_RUNNING},
        {TASK_INIT_RUNNING, EVENT_YIELD, &Task::initYield, TASK_INIT_READY},
    };

    for (auto const& [current_state, condition, action, next_state] : table)
    {
        if ((m_tcb.state == current_state) && (condition == event))
        {
            Port_Core_Disable_PendSV();
            m_tcb.state = next_state;
            if (action != nullptr)
            {
                (this->*action)();
            }
            Port_Core_Enable_PendSV();
            break;
        }
    }
}

void Task::SetDelayTime(uint32_t const ms)
{
    m_tcb.wake_time = BSP_Get_Tick() + ms;
}

uint32_t Task::GetWakeTime(void) const
{
    return m_tcb.wake_time;
}

void Task::readySchedule(void)
{
    TaskManager& task_manager = TaskManager::sGetInstance();
    m_tcb.time_slice = TIME_SLICE_QUANTUM;
    task_manager.SetRunningTaskID(m_tcb.id);
}

void Task::runningYield(void)
{
    TaskManager& task_manager = TaskManager::sGetInstance();
    task_manager.PushToReadyById(m_tcb.id);
    Port_Core_Trigger_PendSV();
}

void Task::runningDelay(void)
{
    Kernel_TaskQ_Enqueue_Sorted_By_Wake_Time(TASK_BLOCKED_DELAY, m_tcb.id);
    Port_Core_Trigger_PendSV();
}

void Task::blockedUnblock(void)
{
    TaskManager& task_manager = TaskManager::sGetInstance();
    task_manager.PushToReadyById(m_tcb.id);
}

void Task::runningSuspend(void)
{
}

void Task::suspendResume(void)
{
}

void Task::runningTerminate(void)
{
    Port_Core_Trigger_PendSV();
}

void Task::terminatedCreate(void)
{
    TaskManager& task_manager = TaskManager::sGetInstance();
    m_tcb.time_slice = TIME_SLICE_QUANTUM;
    task_manager.PushToReadyById(m_tcb.id);
}

void Task::initSchedule()
{
    TaskManager::sGetInstance().SetRunningTaskID(m_tcb.id);
}

void Task::initYield()
{
    Port_Core_Trigger_PendSV();
}

void Task::runningWait()
{
    Port_Core_Trigger_PendSV();
}

bool Task::IsDelayTimeOver(void) const
{
    return m_tcb.wake_time < BSP_Get_Tick();
}

uint32_t Task::GetId() const
{
    return m_tcb.id;
}

uint32_t Task::GetPriority() const
{
    return m_priority;
}

uint32_t Task::GetBasePriority() const
{
    return m_base_priority;
}

void Task::SetPriority(uint32_t priority)
{
    m_priority = priority;
    m_base_priority = priority;
}

void Task::InheritPriority(uint32_t to)
{
    m_priority = to;
}

KernelTaskState_t Task::GetState() const
{
    return m_tcb.state;
}

void Task::DecreaseTimeSlice()
{
    if (this->m_tcb.state == TASK_RUNNING)
    {
        int32_t const remaining = m_tcb.time_slice;
        --m_tcb.time_slice;
        if (remaining < 0)
        {
            this->SetNextState(EVENT_YIELD);
        }
    }
}
