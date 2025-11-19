//
// Created by ��������� on 25. 10. 13..
//

#include "PriorityQueue.hpp"

bool PriorityQueue::IsEmpty() const
{
    return m_bitmap == 0;
}

void PriorityQueue::Push(Task* task)
{
    uint32_t priority = task->GetPriority();
    Push(task, priority);
}

void PriorityQueue::Push(Task* task, uint32_t priority)
{
    Node& node = m_bucket[priority];

    task->SetNextTask(nullptr);

    if (!node.head)
    {
        node.head = node.tail = task;
        set_bit(priority);
    } else
    {
        node.tail->SetNextTask(task);
        node.tail = task;
    }
}

Task* PriorityQueue::Pop()
{
    if (m_bitmap == 0) return nullptr;
    uint32_t priority = getHighestPriorityIndex();
    Node& node = m_bucket[priority];
    Task* task = node.head;
    node.head = task->GetNextTask();
    if (!node.head)
    {
        node.tail = nullptr;
        clr_bit(priority);
    }
    task->SetNextTask(nullptr);
    return task;
}

bool PriorityQueue::Remove(Task* task)
{
    uint32_t priority = task->GetPriority();
    Node& node = m_bucket[priority];
    Task* prev_task = nullptr;

    for (Task* current_task = node.head; current_task; current_task = current_task->GetNextTask())
    {
        if (current_task == task)
        {
            if (prev_task) prev_task->SetNextTask(current_task->GetNextTask());
            else node.head = current_task->GetNextTask();
            if (node.tail == current_task) node.tail = prev_task;
            if (!node.head) clr_bit(priority);
            current_task->SetNextTask(nullptr);
            return true;
        }
        prev_task = current_task;
    }
    return false;
}
