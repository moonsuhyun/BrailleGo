#ifndef KERNEL_KERNEL_H_
#define KERNEL_KERNEL_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

enum
{
    SCG_KERNEL   = 0x0000U,
    SCG_TASK     = 0x0100U,
    SCG_SYSTICK  = 0x0200U,
    SCG_SYNC     = 0x0300U
};

typedef enum SysCallNo
{
    SC_INIT      = SCG_KERNEL  | 0x0001U,
    SC_START     = SCG_KERNEL  | 0x0002U,
    SC_ISRUN     = SCG_KERNEL  | 0x0003U,

    SC_CREATE    = SCG_TASK    | 0x0001U,
    SC_YIELD     = SCG_TASK    | 0x0002U,
    SC_DELAY     = SCG_TASK    | 0x0003U,
    SC_TERMINATE = SCG_TASK    | 0x0004U,

    SC_GETTICK   = SCG_SYSTICK | 0x0001U,

    SC_MUTLOCK   = SCG_SYNC    | 0x0001U,
    SC_MUTUNLOCK = SCG_SYNC    | 0x0002U

} SysCallNo_t;

typedef enum MutexType
{
    MUTEX_UART,
    MUTEX_TYPE_NUM
} MutexType_t;

typedef void (*KernelTaskFunc_t)(void*);

// void Kernel_Init(KernelTaskFunc_t init_task);
void Kernel_Start(KernelTaskFunc_t init_task);
uint32_t Task_Create(void (*start_func)(void*), void* arg, uint32_t priority);
void Task_Yield(void);
void Task_Delay(uint32_t ms);
void Task_Terminate();

bool Mutex_Lock(MutexType_t mutex_type);
bool Mutex_Unlock(MutexType_t mutex_type);

uint32_t SysTick_GetTick(void);
bool Kernel_Is_Running(void);

#ifdef __cplusplus
}
#endif

// user task에서 접근 가능한 kernel service 정의

//void Kernel_Init(void);
//void Kernel_Start(void);
//uint32_t Kernel_Create(void (*start_func)(void));
//void Kernel_Yield(void);
//void Kernel_Delay(uint32_t ms);
//void Kernel_Terminate();

//uint32_t Kernel_Get_SysTick(void);
//bool Kernel_Is_Initialized(void);

//void Kernel_send_events(uint32_t event_list);
//KernelEventFlag_t Kernel_wait_events(uint32_t waiting_list);
//
//bool Kernel_send_msg(KernelMsgQ_t Qname, void* data, uint32_t count);
//uint32_t Kernel_recv_msg(KernelMsgQ_t Qname, void* outdata, uint32_t count);

//void Kernel_lock_sem(void);
//void Kernel_unlock_sem(void);
//
//void Kernel_lock_mutex(void);
//void Kernel_unlock_mutex(void);

#endif // KERNEL_KERNEL_H_
