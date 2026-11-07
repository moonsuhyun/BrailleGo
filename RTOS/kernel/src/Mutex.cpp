//
// Created by ��������� on 25. 9. 19..
//

#include "Mutex.hpp"
#include "Mutex.h"
#include "TaskManager.hpp"

std::array<Mutex, MUTEX_TYPE_NUM> Mutex::m_mutexes{};

int32_t Mutex::Lock()
{
    TaskManager& task_manager = TaskManager::sGetInstance();
    Task* current_task = task_manager.GetRunningTask();
    uint32_t const task_id = current_task->GetId();

    int32_t result = 0;

    if (!m_locked)
    {
        m_locked = true;
        m_owner = task_id;
        m_waiting_count = 0;
        result = 1;
    }
    else
    {
        if (m_owner != task_id)
        {
            // waiting list 추가 및 상태 전이
            m_waiting_count++;
            m_waiting_list.Push(current_task);
            task_manager.MutexWait(m_owner);
        }
    }

    return result;
}

int32_t Mutex::Unlock()
{
    int32_t result = 0;
    if (m_locked)
    {
        TaskManager& task_manager = TaskManager::sGetInstance();

        if (uint32_t const task_id = task_manager.GetRunningTaskId(); m_owner == task_id)
        {
            if (m_waiting_list.IsEmpty())
            {
                m_locked = false;
                m_waiting_count = 0;
                result = 1;
            }
            else
            {
                m_waiting_count--;
                Task* next = m_waiting_list.Pop();
                m_locked = true;
                m_owner = next->GetId();

                task_manager.MutexWake(next);
            }
        }
    }

    return result;
}

int32_t Mutex::MutexLock(MutexType_t mutex_type)
{
    return m_mutexes.at(mutex_type).Lock();
}

int32_t Mutex::MutexUnlock(MutexType_t mutex_type)
{
    return m_mutexes.at(mutex_type).Unlock();
}

int32_t Kernel_Mutex_Lock(MutexType_t mutex_type)
{
    return Mutex::MutexLock(mutex_type);
}

int32_t Kernel_Mutex_Unlock(MutexType_t mutex_type)
{
    return Mutex::MutexUnlock(mutex_type);
}
