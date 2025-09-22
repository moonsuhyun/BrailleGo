//
// Created by ��������� on 25. 9. 19..
//

#ifndef BRAILLEGO_SYNCOBJS_HPP
#define BRAILLEGO_SYNCOBJS_HPP

#include <atomic>

class Mutex
{
private:
        std::atomic<bool> m_locked;
        uint32_t m_owner;
public:
        
};

#endif //BRAILLEGO_SYNCOBJS_HPP