#ifndef KERNEL_KERNEL_H_
#define KERNEL_KERNEL_H_

#include "types.h"

// user task에서 접근 가능한 kernel service 정의

void Kernel_Start(void);
void Kernel_Yield(void);
void Kernel_Delay(uint32_t ms);

uint32_t Kernel_Get_SysTick(void);

void Kernel_send_events(uint32_t event_list);
//KernelEventFlag_t Kernel_wait_events(uint32_t waiting_list);
//
//bool Kernel_send_msg(KernelMsgQ_t Qname, void* data, uint32_t count);
//uint32_t Kernel_recv_msg(KernelMsgQ_t Qname, void* outdata, uint32_t count);

void Kernel_lock_sem(void);
void Kernel_unlock_sem(void);

void Kernel_lock_mutex(void);
void Kernel_unlock_mutex(void);

#endif // KERNEL_KERNEL_H_
