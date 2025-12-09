//
// Created by ��������� on 25. 9. 19..
//

#ifndef KERNEL_INTERNAL_MUTEX_HPP_
#define KERNEL_INTERNAL_MUTEX_HPP_

#include <array>

#include "Kernel.h"
#include "PriorityQueue.hpp"

class Mutex
{
private:
        bool m_locked{false};
        uint32_t m_owner{0};
        uint32_t m_waiting_count{0};
        PriorityQueue m_waiting_list{};
        static std::array<Mutex, MUTEX_TYPE_NUM> m_mutexes;
public:
        uint32_t Lock();
        uint32_t Unlock();
        static uint32_t MutexLock(MutexType_t mutex_type);
        static uint32_t MutexUnlock(MutexType_t mutex_type);
};

#endif // KERNEL_INTERNAL_MUTEX_HPP_