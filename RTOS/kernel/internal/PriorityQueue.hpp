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
    std::array<Node, MAX_PRIORITY_NUM> m_bucket{};
    uint32_t m_bitmap{0};
    inline void set_bit(uint32_t p) { m_bitmap |= (1U << p); }
    inline void clr_bit(uint32_t p) { m_bitmap &= ~(1U << p); }
    inline uint32_t getHighestPriorityIndex() const
    {
        uint32_t leading = __builtin_ctz(m_bitmap);
        return leading;
    }
public:
    bool IsEmpty(void) const;
    void Push(Task* task);
    void Push(Task* task, uint32_t priority);
    Task* Pop(void);
    bool Remove(Task* task);
};

#endif // KERNEL_INTERNAL_PRIORITYQUEUE_HPP_