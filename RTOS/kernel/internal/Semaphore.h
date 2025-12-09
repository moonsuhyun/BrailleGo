//
// Created by ��������� on 25. 12. 8..
//

#ifndef KERNEL_INTERNAL_SEMAPHORE_H_
#define KERNEL_INTERNAL_SEMAPHORE_H_

#ifdef __cplusplus
extern "C" {
#endif

uint32_t Kernel_Semaphore_Create(int32_t initial_count);
uint32_t Kernel_Semaphore_Wait(uint32_t h);
uint32_t Kernel_Semaphore_Signal(uint32_t h);
uint32_t Kernel_Semaphore_GetCount(uint32_t h);

#ifdef __cplusplus
}
#endif

#endif // KERNEL_INTERNAL_SEMAPHORE_H_