//
// Created by ��������� on 25. 10. 13..
//

#ifndef KERNEL_INTERNAL_PRIORITYQUEUE_HPP_
#define KERNEL_INTERNAL_PRIORITYQUEUE_HPP_

#include "Task.hpp"
#include "Types.h"
#include <array>

class PriorityQueue
{
private:
    struct Node
    {
        Task* head{nullptr};
        Task* tail{nullptr};
    };
    std::array<Node, MAX_PRIORITY_NUM> m_bucket{}; // 우선순위별 버킷
    std::array<Task*, MAX_TASK_NUM> m_next{}; // m_next[id]: 같은 버킷에서 id 뒤에 오는 태스크
    uint32_t m_bitmap{0}; // 비어있지 않은 우선순위 비트맵

    Task*& nextOf(Task const* task) { return m_next[task->GetId()]; }
    void set_bit(uint32_t p) { m_bitmap |= (1U << p); }
    void clr_bit(uint32_t p) { m_bitmap &= ~(1U << p); }
    uint32_t getHighestPriorityIndex() const { return __builtin_ctz(m_bitmap); }

public:
    bool  IsEmpty(void) const;
    void  PushBack(Task* task); // task->GetPriority() 버킷의 뒤에 삽입
    void  PushBack(Task* task, uint32_t priority); // 지정 우선순위 버킷의 뒤에 삽입
    Task* Pop(void); // 가장 높은 우선순위 버킷의 앞에서 제거
    bool  Remove(Task const* task); // task->GetPriority() 버킷에서 검색; 우선순위 변경 전에 호출해야 함
};

#endif // KERNEL_INTERNAL_PRIORITYQUEUE_HPP_
