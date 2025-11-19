//
// Created by ��������� on 25. 10. 14..
//

#ifndef BRAILLEGO_MUTEX_H
#define BRAILLEGO_MUTEX_H

#include "Kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

uint32_t Kernel_Mutex_Lock(MutexType_t mutex_type);
uint32_t Kernel_Mutex_Unlock(MutexType_t mutex_type);

#ifdef __cplusplus
}
#endif

#endif //BRAILLEGO_MUTEX_H