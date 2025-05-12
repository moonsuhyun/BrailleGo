#include "stdint.h"

#include "stdio.h"
#include "event.h"

static uint32_t sEventFlag;

void Kernel_event_flag_init(void) {
    sEventFlag = 0;
}

void Kernel_event_flag_set(KernelEventFlag_t event) {
    sEventFlag |= (uint32_t)event;
}

void Kernel_event_flag_clear(KernelEventFlag_t event) {
    sEventFlag &= ~((uint32_t)event);
}

uint32_t Kernel_event_flag_check(KernelEventFlag_t event) {
    if (sEventFlag & (uint32_t)event) {
        Kernel_event_flag_clear(event);
        return 1;
    }
    return 0;
}