
#ifndef KERNEL_INTERNAL_SEMAPHORE_HPP_
#define KERNEL_INTERNAL_SEMAPHORE_HPP_

#include "Types.h"
#include "PriorityQueue.hpp"
#include <array>

using SemHandle = int32_t; // 음수 = 무효 핸들

class Semaphore
{
private:
    bool     m_used{false};
    int32_t  m_count{0};
    uint32_t m_waiting_count{0};
    PriorityQueue m_waiting_list{};

    static std::array<Semaphore, MAX_SEMAPHORE_NUM> s_sems;
    static Semaphore* FromHandle(SemHandle h);

public:
    // 생성: 핸들 or 실패 시 -1
    static SemHandle Create(int32_t initial_count);

    // 동작: 처리 후 카운트, 무효 핸들이면 -EINVAL
    static int32_t Wait(SemHandle h);
    static int32_t Signal(SemHandle h);
    static int32_t GetCount(SemHandle h);
};

#endif // KERNEL_INTERNAL_SEMAPHORE_HPP_
