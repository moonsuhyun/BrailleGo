//
// Created by ��������� on 25. 9. 19..
//

#include "Mutex.hpp"
#include "Mutex.h"
#include "TaskManager.hpp"

#include <errno.h>

std::array<Mutex, MUTEX_TYPE_NUM> Mutex::s_mutexes{};

Mutex& Mutex::Get(MutexType_t const type)
{
    return s_mutexes.at(type);
}

// task_id의 기본 우선순위와 보유 중인 모든 뮤텍스의 대기 태스크 중 최고 우선순위를 비교하여 유효 우선순위를 결정
// prio(T) = min( base(T), min{ TopPriority(M) : M.owner == T } )
void Mutex::UpdatePriority(uint32_t const task_id)
{
    TaskManager& task_manager = TaskManager::sGetInstance();

    uint32_t priority = task_manager.GetBasePriorityById(task_id);

    for (Mutex const& mutex : s_mutexes)
    {
        if (mutex.m_locked && (mutex.m_owner == task_id))
        {
            uint32_t const top = mutex.m_waiters.TopPriority(); // 비어 있으면 UINT32_MAX라 영향 없음
            if (top < priority)
            {
                priority = top;
            }
        }
    }

    task_manager.SetPriorityById(task_id, priority);
}

// task_id가 block되어 기다리는 뮤텍스 번호
int32_t Mutex::FindBlockedOn(uint32_t const task_id)
{
    int32_t found = -1;

    for (uint32_t type = 0; type < MUTEX_TYPE_NUM; ++type)
    {
        if (s_mutexes[type].m_locked && s_mutexes[type].m_waiters.Contains(task_id))
        {
            found = static_cast<int32_t>(type);
            break;
        }
    }

    return found;
}

// Lock에서 type의 소유자에게 대기 태스크의 우선순위를 상속하고, 소유자가 다른 뮤텍스를 대기 중이라면 해당 뮤텍스의 소유자에게도 상속을 전파
int32_t Mutex::Propagate(MutexType_t const type, uint32_t const waiter_id)
{
    TaskManager& task_manager = TaskManager::sGetInstance();

    int32_t result = 0;
    MutexType_t current = type;

    for (uint32_t depth = 0; depth < MUTEX_TYPE_NUM; ++depth) // 체인 길이는 최대 전체 뮤텍스 수까지
    {
        Mutex const& mutex = Get(current);
        uint32_t const owner = mutex.m_owner;

        if (owner == waiter_id)
        {
            result = -EDEADLK; // Deadlock
            break;
        }

        uint32_t const needed = mutex.m_waiters.TopPriority();
        if (needed >= task_manager.GetPriorityById(owner))
        {
            break; // 소유자의 우선순위가 이미 충분히 높음
        }

        task_manager.SetPriorityById(owner, needed);

        int32_t const next = FindBlockedOn(owner);
        if (next < 0)
        {
            break; // 소유자가 대기 중인 mutex가 없음
        }
        current = static_cast<MutexType_t>(next);
    }

    return result;
}

int32_t Mutex::Lock(MutexType_t const type)
{
    TaskManager& task_manager = TaskManager::sGetInstance();
    Mutex& mutex = Get(type);
    uint32_t const task_id = task_manager.GetRunningTaskId();

    int32_t result = 0;

    if (!mutex.m_locked)
    {
        // 즉시 획득
        mutex.m_locked = true;
        mutex.m_owner = task_id;
        result = 1;
    }
    else
    {
        if (mutex.m_owner != task_id)
        {
            // 대기
            mutex.m_waiters.PushBack(task_id); // Propagate에서 waiters.TopPriority() 기준으로 전파하므로 리스트 먼저 등록해야 함

            result = Propagate(type, task_id);
            if (result < 0)
            {
                mutex.m_waiters.Remove(task_id); // Deadlock; 블록하지 않고 에러 반환
            }
            else
            {
                task_manager.TaskWait();
            }
        }
        // else: 재귀 잠금은 무시
    }

    return result;
}

int32_t Mutex::Unlock(MutexType_t const type)
{
    TaskManager& task_manager = TaskManager::sGetInstance();
    Mutex& mutex = Get(type);
    uint32_t const task_id = task_manager.GetRunningTaskId();

    int32_t result = 0;

    if (mutex.m_locked && (mutex.m_owner == task_id))
    {
        int32_t const next = mutex.m_waiters.PopHighest();

        if (next < 0)
        {
            // 다른 대기자 없음
            mutex.m_locked = false;
        }
        else
        {
            // 다음 우선순위의 대기자(next)에게 소유권 이전
            auto const next_id = static_cast<uint32_t>(next);
            mutex.m_owner = next_id;
            // next가 쥐고있는 다른 뮤텍스의 대기자로부터 받을 상속이 있으면, Wake 전에 반영해야 ready 큐의 제 버킷에 들어감
            UpdatePriority(next_id);
            task_manager.TaskWake(next_id);
        }

        UpdatePriority(task_id); // 아직 쥐고 있는 무텍스에 높은 대기자가 있으면 그만큼 유지, 없으면 base로 복귀

        // 커널 구조상 time slice가 끝나야만 선점이 이루어지므로, 넘겨받은 태스크 우선순위가 더 높으면 즉시 yield 해야함
        if ((next >= 0) && (task_manager.GetPriorityById(static_cast<uint32_t>(next)) < task_manager.GetPriorityById(task_id)))
        {
            task_manager.TaskYield();
        }

        result = 1;
    }
    // else: 잠겨있지 않거나 소유자가 아님

    return result;
}

int32_t Kernel_Mutex_Lock(MutexType_t const mutex_type)
{
    return Mutex::Lock(mutex_type);
}

int32_t Kernel_Mutex_Unlock(MutexType_t const mutex_type)
{
    return Mutex::Unlock(mutex_type);
}
