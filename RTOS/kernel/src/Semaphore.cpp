#include "Semaphore.hpp"
#include "Semaphore.h"
#include "TaskManager.hpp"

std::array<Semaphore, MAX_SEMAPHORE_NUM> Semaphore::s_sems{};

Semaphore* Semaphore::FromHandle(SemHandle h)
{
    if (h >= MAX_SEMAPHORE_NUM) return nullptr;
    if (!s_sems[h].m_used)      return nullptr;
    return &s_sems[h];
}


SemHandle Semaphore::Create(int32_t initial_count)
{
    for (SemHandle i = 0; i < MAX_SEMAPHORE_NUM; ++i) {
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

void Semaphore::Destroy(SemHandle h)
{
    if (auto* sem = FromHandle(h)) {
        sem->m_used = false;
        sem->m_count = 0;
        sem->m_waiting_count = 0;
    }
}

void Semaphore::Wait(SemHandle h)
{
    Semaphore* sem = FromHandle(h);

    if (sem->m_count > 0) {
        sem->m_count--;
        return;
    }

    TaskManager task_manager = TaskManager::sGetInstance();
    Task* current_task = task_manager.GetRunningTask();

    sem->m_waiting_count++;
    sem->m_waiting_list.Push(current_task);
    task_manager.SemWait();
}

void Semaphore::Signal(SemHandle h)
{
    auto* sem = FromHandle(h);

    if (sem->m_waiting_count > 0)
        {
        sem->m_waiting_count--;
        Task* next = sem->m_waiting_list.Pop();

        TaskManager::sGetInstance().SemWake(next);
    }
    else
    {
        sem->m_count++;
    }
}

uint32_t Semaphore::GetCount(SemHandle h)
{
    auto* sem = FromHandle(h);
    return sem->m_count;
}

uint32_t Kernel_Semaphore_Create(int32_t initial_count)
{
    return Semaphore::Create(initial_count);
}

uint32_t Kernel_Semaphore_Wait(uint32_t h)
{
    Semaphore::Wait(h);
    return 0;
}

uint32_t Kernel_Semaphore_Signal(uint32_t h)
{
    Semaphore::Signal(h);
    return 0;
}

uint32_t Kernel_Semaphore_GetCount(uint32_t h)
{
    return Semaphore::GetCount(h);
}
