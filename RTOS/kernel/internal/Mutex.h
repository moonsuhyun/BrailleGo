//
// Created by ��������� on 25. 10. 14..
//

#ifndef KERNEL_INTERNAL_MUTEX_H_
#define KERNEL_INTERNAL_MUTEX_H_

#include "Kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

// Lock:   1 = 즉시 획득, 0 = 대기 후 획득, -EDEADLK = Deadlock 감지
// Unlock: 1 = 해제됨, 0 = 소유자가 아님
int32_t Kernel_Mutex_Lock(MutexType_t mutex_type);
int32_t Kernel_Mutex_Unlock(MutexType_t mutex_type);

#ifdef __cplusplus
}
#endif

#endif // KERNEL_INTERNAL_MUTEX_H_