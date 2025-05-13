#include "stdint.h"
#include "stdbool.h"

#include "kernel.h"
#include "memio.h"
#include "synch.h"

void Kernel_start(void) {
    Kernel_task_start();
}

void Kernel_yield(void) {
    Kernel_task_scheduler();
}

void Kernel_send_events(uint32_t event_list) {
    for (uint32_t i=0; i<32; i++) {
        if ((event_list>>i) & 1) {
            KernelEventFlag_t sending_event = KernelEventFlag_Empty;
            sending_event = (KernelEventFlag_t)SET_BIT(sending_event, i);
            Kernel_event_flag_set(sending_event);
        }
    }
}

KernelEventFlag_t Kernel_wait_events(uint32_t waiting_list) {
    for (uint32_t i=0; i<32; i++) {
        if ((waiting_list>>i) & 1) {
            KernelEventFlag_t waiting_event = KernelEventFlag_Empty;
            waiting_event = (KernelEventFlag_t)SET_BIT(waiting_event, i);
            if (Kernel_event_flag_check(waiting_event)) return waiting_event;
        }
    }
    return KernelEventFlag_Empty;
}

bool Kernel_send_msg(KernelMsgQ_t Qname, void* data, uint32_t count) {
    uint8_t* d = (uint8_t*) data;
    for (uint32_t i=0; i<count; i++) {
        if (false == Kernel_msgQ_enqueue(Qname, *d++)) {
            for (uint32_t j=0; j<i; j++) {
                uint8_t rollback;
                Kernel_msgQ_dequeue(Qname, &rollback);
            }
            return false;
        }
    }
    return true;
}

uint32_t Kernel_recv_msg(KernelMsgQ_t Qname, void* outdata, uint32_t count) {
    uint8_t* d = (uint8_t*) outdata;
    for (uint32_t i=0; i<count; i++) {
        if (!Kernel_msgQ_dequeue(Qname, d++)) return i;
    }
    return count;
}

void Kernel_lock_sem(void) {
    while (!Kernel_sem_test()) Kernel_yield();
}

void Kernel_unlock_sem(void) {
    Kernel_sem_release();
}

void Kernel_lock_mutex(void) {
    while (1) {
        uint32_t taskId = Kernel_task_get_current_task_id();
        if (!Kernel_mutex_lock(taskId)) Kernel_yield();
        else break;
    }
}
void Kernel_unlock_mutex(void) {
    uint32_t taskId = Kernel_task_get_current_task_id();
    if (!Kernel_mutex_unlock(taskId)) Kernel_yield();
}