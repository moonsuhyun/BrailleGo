
#include "WaitList.hpp"
#include "TaskManager.hpp"

#include <stdint.h>

// 상속으로 인해 task의 우선순위가 리스트 외부에서 바뀌므로 매번 재 탐색 필요함
uint32_t WaitList::highestIndex(void) const
{
    TaskManager const& task_manager = TaskManager::sGetInstance();

    uint32_t best_index = m_count;
    uint32_t best_priority = UINT32_MAX;

    for (uint32_t i = 0; i < m_count; ++i)
    {
        if (uint32_t const priority = task_manager.GetPriorityById(m_ids[i]);
            priority < best_priority) // 같은 값이면 먼저 들어온 쪽 유지
        {
            best_priority = priority;
            best_index = i;
        }
    }

    return best_index;
}

bool WaitList::IsEmpty(void) const
{
    return m_count == 0U;
}

void WaitList::PushBack(uint32_t const id)
{
    if (m_count < MAX_TASK_NUM)
    {
        m_ids[m_count] = static_cast<uint8_t>(id);
        m_count++;
    }
}

int32_t WaitList::PopHighest(void)
{
    int32_t id = -1;

    if (uint32_t const index = highestIndex(); index < m_count)
    {
        id = m_ids[index];
        for (uint32_t i = index + 1U; i < m_count; ++i)
        {
            m_ids[i - 1U] = m_ids[i]; // 뒤 항목을 당겨 FIFO 순서 유지
        }
        --m_count;
    }

    return id;
}

uint32_t WaitList::TopPriority(void) const
{
    uint32_t priority = UINT32_MAX;

    if (uint32_t const index = highestIndex(); index < m_count)
    {
        priority = TaskManager::sGetInstance().GetPriorityById(m_ids[index]);
    }

    return priority;
}

bool WaitList::Contains(uint32_t const id) const
{
    bool found = false;

    for (uint32_t i = 0; i < m_count; ++i)
    {
        if (m_ids[i] == id)
        {
            found = true;
            break;
        }
    }

    return found;
}

bool WaitList::Remove(uint32_t const id)
{
    bool removed = false;

    for (uint32_t i = 0; i < m_count; ++i)
    {
        if (m_ids[i] == id)
        {
            for (uint32_t j = i + 1U; j < m_count; ++j)
            {
                m_ids[j - 1U] = m_ids[j];
            }
            --m_count;
            removed = true;
            break;
        }
    }

    return removed;
}
