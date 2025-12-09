//
// Created by ��������� on 25. 10. 14..
//

#ifndef KERNEL_INTERNAL_MUTEX_H_
#define KERNEL_INTERNAL_MUTEX_H_

#include "Kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

uint32_t Kernel_Mutex_Lock(MutexType_t mutex_type);
uint32_t Kernel_Mutex_Unlock(MutexType_t mutex_type);

#ifdef __cplusplus
}
#endif

#endif // KERNEL_INTERNAL_MUTEX_H_