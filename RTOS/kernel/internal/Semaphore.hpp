
#ifndef KERNEL_INTERNAL_SEMAPHORE_HPP_
#define KERNEL_INTERNAL_SEMAPHORE_HPP_

#include "Types.h"
#include "PriorityQueue.hpp"
#include <array>

using SemHandle = uint32_t;

class Semaphore
{
private:
    bool     m_used{false};
    uint32_t m_count{0};
    uint32_t m_waiting_count{0};
    PriorityQueue m_waiting_list{};
    static std::array<Semaphore, MAX_SEMAPHORE_NUM> s_sems;
    static Semaphore* FromHandle(SemHandle h);
public:
    static SemHandle Create(int32_t initial_count);
    static void      Destroy(SemHandle h);

    static uint32_t Wait(SemHandle h);
    static uint32_t Signal(SemHandle h);
    static uint32_t GetCount(SemHandle h);
};

#endif // KERNEL_INTERNAL_SEMAPHORE_HPP_