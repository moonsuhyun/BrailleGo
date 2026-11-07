//
// Created by ��������� on 25. 10. 14..
//

#ifndef KERNEL_INTERNAL_MUTEX_H_
#define KERNEL_INTERNAL_MUTEX_H_

#include "Kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

// 반환값: 1 = 즉시 처리됨, 0 = 대기 or 소유자 아님
int32_t Kernel_Mutex_Lock(MutexType_t mutex_type);
int32_t Kernel_Mutex_Unlock(MutexType_t mutex_type);

#ifdef __cplusplus
}
#endif

#endif // KERNEL_INTERNAL_MUTEX_H_