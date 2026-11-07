#ifndef KERNEL_INTERNAL_TASK_H_
#define KERNEL_INTERNAL_TASK_H_

#include "Types.h"

class Task
{
private:
    uint32_t* m_stack_pointer;
    uint8_t* m_stack_base;
    int32_t m_time_slice;
    uint32_t m_wake_time;
    KernelTaskState_t m_state;
    uint32_t m_id;

    // 우선순위
    uint32_t m_priority; // 상속으로 바뀔 수 있는 실시간 우선순위
    uint32_t m_base_priority; // 생성시 fix되는 기본 우선순위

    // ready 큐 링크 (PriorityQueue가 사용)
    Task* m_next_task{nullptr};

    // 상태 전이 테이블과 액션
    typedef struct KernelEventActionTable
    {
        KernelTaskState_t current_state;
        KernelTaskEvent_t event;
        void (Task::*action)(void);
        KernelTaskState_t next_state;
    } KernelEventActionTable_t;

    void readySchedule(void);
    void runningYield(void);
    void runningDelay(void);
    void runningWait(void);
    void runningSuspend(void);
    void runningTerminate(void);
    void blockedUnblock(void);
    void suspendResume(void);
    void terminatedCreate(void);
    void initSchedule(void);
    void initYield(void);

public:
    Task()
    {
    }

    // 생성, 초기화
    void Init(uint32_t* stack_pointer, uint8_t* stack_base, uint32_t id);
    void SetProgramCounter(uint32_t wrapper, uint32_t pc, void* arg);

    // 상태 전이
    void SetNextState(KernelTaskEvent_t event);

    // 타임슬라이스, 딜레이
    void DecreaseTimeSlice(void);
    void SetDelayTime(uint32_t ms);
    uint32_t GetWakeTime(void) const;
    bool IsDelayTimeOver(void) const;

    // 우선순위
    void SetPriority(uint32_t priority); // base와 현재를 함께 설정
    void InheritPriority(uint32_t to); // 현재만 변경
    void RestorePriority(void); // 현재 = base
    uint32_t GetPriority(void) const;

    // ready 큐 링크
    void SetNextTask(Task* task);
    Task* GetNextTask(void) const;

    // 조회
    uint32_t GetId(void) const;
    KernelTaskState_t GetState(void) const;
};

#endif // KERNEL_INTERNAL_TASK_H_
