
#ifndef KERNEL_INTERNAL_WAITLIST_HPP_
#define KERNEL_INTERNAL_WAITLIST_HPP_

#include "Types.h"

class WaitList
{
private:
    uint8_t m_ids[MAX_TASK_NUM]{};
    uint8_t m_count{0};

    uint32_t highestIndex(void) const; // 우선순위 최소값(가장 높음)인 첫 항목의 인덱스

public:
    bool     IsEmpty(void) const;
    void     PushBack(uint32_t id);
    int32_t  PopHighest(void); // 가장 높은 우선순위 태스크 제거 후 ID 반환
    uint32_t TopPriority(void) const; // 가장 높은 우선순위 값; 비어 있을땐 UINT32_MAX 반환
    bool     Contains(uint32_t id) const;
    bool     Remove(uint32_t id);
};

#endif // KERNEL_INTERNAL_WAITLIST_HPP_
