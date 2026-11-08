#include "Semaphore.hpp"
#include "Semaphore.h"
#include "TaskManager.hpp"
#include <errno.h>

std::array<Semaphore, MAX_SEMAPHORE_NUM> Semaphore::s_sems{};

Semaphore* Semaphore::FromHandle(SemHandle const h)
{
    Semaphore* sem = nullptr;

    if ((h < 0) || (h >= static_cast<SemHandle>(MAX_SEMAPHORE_NUM)))
    {
        sem = nullptr;
    }
    else
    {
        if (!s_sems[h].m_used)
        {
            sem = nullptr;
        }
        else
        {
            sem = &s_sems[h];
        }
    }

    return sem;
}


SemHandle Semaphore::Create(int32_t const initial_count)
{
    int32_t handle = -1;

    for (SemHandle i = 0; i < static_cast<SemHandle>(MAX_SEMAPHORE_NUM); ++i)
    {
        if (!s_sems[i].m_used)
        {
            auto& s = s_sems[i];
            s.m_used = true;
            s.m_count = initial_count;
            handle = i;
            break;
        }
    }

    return handle;
}

int32_t Semaphore::Wait(SemHandle const h)
{
    Semaphore* sem = FromHandle(h);

    int32_t count = -EINVAL;

    if (sem != nullptr)
    {
        if (sem->m_count > 0)
        {
            sem->m_count--;
            count = sem->m_count;
        }
        else
        {
            TaskManager& task_manager = TaskManager::sGetInstance();

            sem->m_waiters.PushBack(task_manager.GetRunningTaskId());
            task_manager.TaskWait();

            count = sem->m_count;
        }
    }

    return count;
}

int32_t Semaphore::Signal(SemHandle const h)
{
    Semaphore* sem = FromHandle(h);

    int32_t count = -EINVAL;

    if (sem != nullptr)
    {
        if (!sem->m_waiters.IsEmpty())
        {
            int32_t const next = sem->m_waiters.PopHighest();
            TaskManager::sGetInstance().TaskWake(static_cast<uint32_t>(next));
            count = sem->m_count;
        }
        else
        {
            sem->m_count++;
            count = sem->m_count;
        }
    }

    return count;
}

int32_t Semaphore::GetCount(SemHandle const h)
{
    Semaphore const* sem = FromHandle(h);

    int32_t count = -EINVAL;

    if (sem != nullptr)
    {
        count = sem->m_count;
    }

    return count;
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
