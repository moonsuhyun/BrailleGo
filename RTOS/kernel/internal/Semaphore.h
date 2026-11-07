//
// Created by ��������� on 25. 12. 8..
//

#ifndef KERNEL_INTERNAL_SEMAPHORE_H_
#define KERNEL_INTERNAL_SEMAPHORE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

// 반환값: 핸들 또는 카운트, 실패 시 음수(-errno)
int32_t Kernel_Semaphore_Create(int32_t initial_count);
int32_t Kernel_Semaphore_Wait(int32_t h);
int32_t Kernel_Semaphore_Signal(int32_t h);
int32_t Kernel_Semaphore_GetCount(int32_t h);

#ifdef __cplusplus
}
#endif

#endif // KERNEL_INTERNAL_SEMAPHORE_H_