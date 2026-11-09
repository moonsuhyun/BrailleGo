#ifndef KERNEL_INTERNAL_TASK_H_
#define KERNEL_INTERNAL_TASK_H_

#include "Types.h"

#include <type_traits>

class Task
{
    friend class TaskManager; // TaskManager에서만 Task 내부 상태를 변경

private:
    // Port에서 포인터(KernelTcb_t*)로 접근하는 영역이므로 반드시 첫 멤버여야 함
    KernelTcb_t m_tcb;

    // 우선순위
    uint32_t m_priority; // 상속으로 바뀔 수 있는 실시간 우선순위
    uint32_t m_base_priority; // 생성시 fix되는 기본 우선순위

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

    // TaskManager에서 호출하는 Task 내부 상태 변경 메서드
    // 생성, 초기화
    void Init(uint32_t* stack_pointer, uint8_t* stack_base, uint32_t id);
    void SetProgramCounter(uint32_t wrapper, uint32_t pc, void* arg);
    // 상태 전이
    void SetNextState(KernelTaskEvent_t event);
    // 타임슬라이스, 딜레이
    void DecreaseTimeSlice(void);
    void SetDelayTime(uint32_t ms);
    // 우선순위
    void SetPriority(uint32_t priority); // base와 현재를 함께 설정
    void InheritPriority(uint32_t to); // 현재만 변경
    // TCB 접근(Port 계층 전달)
    KernelTcb_t* GetTcb(void) { return &m_tcb; }

public:
    Task()
    {
    }

    // const 조회
    uint32_t GetId(void) const;
    uint32_t GetPriority(void) const;
    uint32_t GetBasePriority(void) const;
    KernelTaskState_t GetState(void) const;
    uint32_t GetWakeTime(void) const;
    bool IsDelayTimeOver(void) const;
};

// standard-layout 클래스는 첫 멤버가 오프셋 0에 놓이므로 m_tcb를 첫 멤버로 유지하는 한 Task* 를 KernelTcb_t* 로 그대로 사용 가능
static_assert(std::is_standard_layout_v<Task>, "Task must be standard-layout");

#endif // KERNEL_INTERNAL_TASK_H_
