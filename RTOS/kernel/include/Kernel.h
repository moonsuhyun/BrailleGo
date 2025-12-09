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
    SC_MUTUNLOCK = SCG_SYNC    | 0x0002U,
    SC_SEMCREATE = SCG_SYNC    | 0x0003U,
    SC_SEMWAIT   = SCG_SYNC    | 0x0004U,
    SC_SEMSIGNAL = SCG_SYNC    | 0x0005U,
    SC_SEMGETCNT = SCG_SYNC    | 0x0006U

} SysCallNo_t;

typedef enum MutexType
{
    MUTEX_UART,
    MUTEX_TYPE_NUM
} MutexType_t;

typedef uint32_t SemHandle_t;

typedef void (*KernelTaskFunc_t)(void*);

void Kernel_Start(KernelTaskFunc_t init_task);
uint32_t Task_Create(void (*start_func)(void*), void* arg, uint32_t priority);
void Task_Yield(void);
void Task_Delay(uint32_t ms);
void Task_Terminate();

void Mutex_Lock(MutexType_t mutex_type);
void Mutex_Unlock(MutexType_t mutex_type);

SemHandle_t Semaphore_Create(int32_t initial_count);
void Semaphore_Wait(SemHandle_t h);
void Semaphore_Signal(SemHandle_t h);
int32_t Semaphore_GetCount(SemHandle_t h);

uint32_t SysTick_GetTick(void);
bool Kernel_Is_Running(void);

int32_t UART_Read(uint8_t* buf, uint32_t len);
int32_t UART_ReadLine(char* buf, uint32_t max_len);

#ifdef __cplusplus
}
#endif

#endif // KERNEL_KERNEL_H_
