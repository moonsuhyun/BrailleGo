//
// Created by ��������� on 25. 9. 19..
//

#include "Mutex.hpp"
#include "Mutex.h"
#include "TaskManager.hpp"

std::array<Mutex, MUTEX_TYPE_NUM> Mutex::m_mutexes{};

uint32_t Mutex::Lock()
{
    TaskManager& task_manager = TaskManager::sGetInstance();
    uint32_t task_id = task_manager.GetRunningTaskId();

    if (!m_locked)
    {
        m_locked = true;
        m_owner = task_id;
        m_waiting_count = 0;
        return true;
    }

    if (m_owner != task_id)
    {
        // waiting list 추가 및 상태 전이
        m_waiting_list.at(m_waiting_count++) = task_id;

        task_manager.MutexWait(m_owner);
    }
    return false;
}

uint32_t Mutex::Unlock()
{
    if (m_locked)
    {
        TaskManager& task_manager = TaskManager::sGetInstance();
        uint32_t task_id = task_manager.GetRunningTaskId();
        if (m_owner == task_id)
        {
            m_locked = false;
            // waiting list 상태전이
            task_manager.MutexWake(m_waiting_list, m_waiting_count);
            m_waiting_count = 0;
            return true;
        }

        return false;
    }
    return false;
}

uint32_t Mutex::MutexLock(MutexType_t mutex_type)
{
    return m_mutexes.at(mutex_type).Lock();
}

uint32_t Mutex::MutexUnlock(MutexType_t mutex_type)
{
    return m_mutexes.at(mutex_type).Unlock();
}

uint32_t Kernel_Mutex_Lock(MutexType_t mutex_type)
{
    return Mutex::MutexLock(mutex_type);
}

uint32_t Kernel_Mutex_Unlock(MutexType_t mutex_type)
{
    return Mutex::MutexUnlock(mutex_type);
}
