#include "msg.h"

KernelMsgCirQ_t sMsgQ[KernelMsgQ_Num];

void Kernel_msgQ_init(void) {
    for (uint32_t i=0; i<KernelMsgQ_Num; i++) {
        sMsgQ[i].front = 0;
        sMsgQ[i].rear = 0;
        memset(sMsgQ[i].Queue, 0, MSG_Q_SIZE_BYTE * sizeof(uint8_t));
    }
}

bool Kernel_msgQ_is_empty(KernelMsgQ_t Qname) {
    if (Qname >= KernelMsgQ_Num) return false;
    if (sMsgQ[Qname].front == sMsgQ[Qname].rear) return true;
    return false;
}

bool Kernel_msgQ_is_full(KernelMsgQ_t Qname) {
    if (Qname >= KernelMsgQ_Num) return false;
    if (((sMsgQ[Qname].rear+1)%MSG_Q_SIZE_BYTE) == sMsgQ[Qname].front) return true;
    return false;
}

bool Kernel_msgQ_enqueue(KernelMsgQ_t Qname, uint8_t data) {
    if (Qname >= KernelMsgQ_Num) return false;
    if (Kernel_msgQ_is_full(Qname)) return false;

    sMsgQ[Qname].rear++;
    sMsgQ[Qname].rear %= MSG_Q_SIZE_BYTE;

    uint32_t idx = sMsgQ[Qname].rear;
    sMsgQ[Qname].Queue[idx] = data;

    return true;
}

bool Kernel_msgQ_dequeue(KernelMsgQ_t Qname, uint8_t* outdata) {
    if (Qname >= KernelMsgQ_Num) return false;
    if (Kernel_msgQ_is_empty(Qname)) return false;

    sMsgQ[Qname].front++;
    sMsgQ[Qname].front %= MSG_Q_SIZE_BYTE;

    uint32_t idx = sMsgQ[Qname].front;
    *outdata = sMsgQ[Qname].Queue[idx];

    sMsgQ[Qname].Queue[idx] = 0;

    return true;
}
