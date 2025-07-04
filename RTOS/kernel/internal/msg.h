#include "devlib.h"
#include "stdint.h"
#include "stdbool.h"
#include "memory.h"

#ifndef KERNEL_MSG_H_
#define KERNEL_MSG_H_

#define MSG_Q_SIZE_BYTE 512

typedef enum KernelMsgQ {
    KernelMsgQ_Task0,
    KernelMsgQ_Task1,
    KernelMsgQ_Task2,
    KernelMsgQ_Num
} KernelMsgQ_t;

typedef struct KernelMsgCirQ {
    uint32_t front;
    uint32_t rear;
    uint8_t Queue[MSG_Q_SIZE_BYTE];
} KernelMsgCirQ_t;

void Kernel_msgQ_init(void);
bool Kernel_msgQ_is_empty(KernelMsgQ_t Qname);
bool Kernel_msgQ_is_full(KernelMsgQ_t Qname);
bool Kernel_msgQ_enqueue(KernelMsgQ_t Qname, uint8_t data);
bool Kernel_msgQ_dequeue(KernelMsgQ_t Qname, uint8_t* outdata);

#endif // KERNEL_MSG_H_
