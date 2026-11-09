//
// Created by ��������� on 25. 9. 19..
//

#ifndef KERNEL_INTERNAL_MUTEX_HPP_
#define KERNEL_INTERNAL_MUTEX_HPP_

#include <array>

#include "Kernel.h"
#include "WaitList.hpp"

class Mutex
{
private:
    bool     m_locked{false};
    uint32_t m_owner{0};
    WaitList m_waiters{};

    static std::array<Mutex, MUTEX_TYPE_NUM> s_mutexes;

    static Mutex&  Get(MutexType_t type);
    static void    UpdatePriority(uint32_t task_id); // task_id의 유효 우선순위 재계산
    static int32_t FindBlockedOn(uint32_t task_id); // task_id가 대기 중인 뮤텍스, 없으면 -1
    static int32_t Propagate(MutexType_t type, uint32_t waiter_id); // 연쇄 상속

public:
    // Lock:   1 = 즉시 획득, 0 = 대기 후 획득, -EDEADLK = Deadlock 감지
    // Unlock: 1 = 해제됨, 0 = 소유자가 아님
    static int32_t Lock(MutexType_t type);
    static int32_t Unlock(MutexType_t type);
};

#endif // KERNEL_INTERNAL_MUTEX_HPP_
