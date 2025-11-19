//
// Created by ��������� on 25. 9. 19..
//

#ifndef BRAILLEGO_SYNCOBJS_HPP
#define BRAILLEGO_SYNCOBJS_HPP

#include <array>

#include "types.h"
#include "Kernel.h"

class Mutex
{
private:
        bool m_locked{false};
        uint32_t m_owner{0};
        uint32_t m_waiting_count{0};
        std::array<uint32_t, MAX_TASK_NUM> m_waiting_list{};
        static std::array<Mutex, MUTEX_TYPE_NUM> m_mutexes;
public:
        uint32_t Lock();
        uint32_t Unlock();
        static uint32_t MutexLock(MutexType_t mutex_type);
        static uint32_t MutexUnlock(MutexType_t mutex_type);
};

#endif //BRAILLEGO_SYNCOBJS_HPP