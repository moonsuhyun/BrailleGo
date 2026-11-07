#include "Semaphore.hpp"
#include "Semaphore.h"
#include "TaskManager.hpp"
#include <errno.h>

std::array<Semaphore, MAX_SEMAPHORE_NUM> Semaphore::s_sems{};

Semaphore* Semaphore::FromHandle(SemHandle h)
{
    if (h < 0 || h >= static_cast<SemHandle>(MAX_SEMAPHORE_NUM)) return nullptr;
    if (!s_sems[h].m_used)      return nullptr;
    return &s_sems[h];
}


SemHandle Semaphore::Create(int32_t initial_count)
{
    for (SemHandle i = 0; i < static_cast<SemHandle>(MAX_SEMAPHORE_NUM); ++i) {
        if (!s_sems[i].m_used) {
            auto& s = s_sems[i];
            s.m_used = true;
            s.m_count = initial_count;
            s.m_waiting_count = 0;
            return i;
        }
    }
    return -1;
}

int32_t Semaphore::Wait(SemHandle h)
{
    Semaphore* sem = FromHandle(h);
    if (sem == nullptr) return -EINVAL;

    if (sem->m_count > 0) {
        sem->m_count--;
        return sem->m_count;
    }

    TaskManager& task_manager = TaskManager::sGetInstance();
    Task* current_task = task_manager.GetRunningTask();

    sem->m_waiting_count++;
    sem->m_waiting_list.Push(current_task);
    task_manager.SemWait();

    return sem->m_count;
}

int32_t Semaphore::Signal(SemHandle h)
{
    auto* sem = FromHandle(h);
    if (sem == nullptr) return -EINVAL;

    if (sem->m_waiting_count > 0)
    {
        sem->m_waiting_count--;
        Task* next = sem->m_waiting_list.Pop();

        TaskManager::sGetInstance().SemWake(next);

        return sem->m_count;
    }
    else
    {
        sem->m_count++;
        return sem->m_count;
    }
}

int32_t Semaphore::GetCount(SemHandle h)
{
    auto* sem = FromHandle(h);
    if (sem == nullptr) return -EINVAL;
    return sem->m_count;
}

int32_t Kernel_Semaphore_Create(int32_t initial_count)
{
    return Semaphore::Create(initial_count);
}

int32_t Kernel_Semaphore_Wait(int32_t h)
{
    return Semaphore::Wait(h);
}

int32_t Kernel_Semaphore_Signal(int32_t h)
{
    return Semaphore::Signal(h);
}

int32_t Kernel_Semaphore_GetCount(int32_t h)
{
    return Semaphore::GetCount(h);
}
