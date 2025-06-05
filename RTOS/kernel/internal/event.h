#include "stdbool.h"

#ifndef KERNEL_EVENT_H_
#define KERNEL_EVENT_H_

typedef enum KernelEventFlag {
  KernelEventFlag_UartIn  = 0x00000001,
  KernelEventFlag_CmdIn   = 0x00000002,
  KernelEventFlag_CmdOut  = 0x00000004,
  KernelEventFlag_Unlock  = 0x00000008,
  KernelEventFlag_Empty   = 0x00000000
} KernelEventFlag_t;

void Kernel_event_flag_init(void);
void Kernel_event_flag_set(KernelEventFlag_t event);
void Kernel_event_flag_clear(KernelEventFlag_t event);
bool Kernel_event_flag_check(KernelEventFlag_t evnet);

#endif // KERNEL_EVENT_H_
