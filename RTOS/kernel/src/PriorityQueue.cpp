//
// Created by ��������� on 25. 10. 13..
//

#include "PriorityQueue.hpp"

bool PriorityQueue::IsEmpty() const
{
    return m_bitmap == 0;
}

void PriorityQueue::PushBack(Task* task)
{
    uint32_t const priority = task->GetPriority();
    auto& [head, tail] = m_bucket[priority];

    nextOf(task) = nullptr;

    if (head == nullptr)
    {
        head = tail = task;
        set_bit(priority);
    }
    else
    {
        nextOf(tail) = task;
        tail = task;
    }
}

Task* PriorityQueue::Pop()
{
    Task* task = nullptr;

    if (m_bitmap != 0U)
    {
        uint32_t const priority = getHighestPriorityIndex();
        auto& [head, tail] = m_bucket[priority];

        task = head;
        head = nextOf(task);

        if (head == nullptr)
        {
            tail = nullptr;
            clr_bit(priority);
        }

        nextOf(task) = nullptr;
    }

    return task;
}

bool PriorityQueue::Remove(Task const* task)
{
    uint32_t const priority = task->GetPriority();
    auto& [head, tail] = m_bucket[priority];

    Task* prev = nullptr;
    bool result = false;

    for (Task* cur = head; cur; cur = nextOf(cur))
    {
        if (cur == task)
        {
            if (prev != nullptr)
            {
                nextOf(prev) = nextOf(cur);
            }
            else
            {
                head = nextOf(cur);
            }

            if (tail == cur)
            {
                tail = prev;
            }

            if (head == nullptr)
            {
                clr_bit(priority);
            }

            nextOf(cur) = nullptr;
            result = true;
            break;
        }

        prev = cur;
    }

    return result;
}
