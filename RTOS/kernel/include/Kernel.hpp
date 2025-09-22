/*
 * Kernel.hpp
 *
 *  Created on: Jul 9, 2025
 *      Author: moon
 */

#ifndef KERNEL_INCLUDE_KERNEL_HPP_
#define KERNEL_INCLUDE_KERNEL_HPP_

#include <stdint.h>

class Kernel {
private:
	bool m_is_running;
	Kernel();
public:
	void Init(void);
	void Start(void);
	uint32_t Create(void (*start_func)(void*), void* arg);
	void Yield(void);
	void Delay(uint32_t ms);
	void Terminate(void);
	uint32_t GetSysTick(void);
	bool IsRunning(void);
	static Kernel& sGetInstance(void);
};

#ifdef __cplusplus
extern "C" {
#endif

void Kernel_Init(void);
void Kernel_Start(void);
uint32_t Kernel_Create(void (*start_func)(void*), void* arg);
void Kernel_Yield(void);
void Kernel_Delay(uint32_t ms);
void Kernel_Terminate();

uint32_t Kernel_Get_SysTick(void);
bool Kernel_Is_Running(void);

#ifdef __cplusplus
}
#endif

#endif /* KERNEL_INCLUDE_KERNEL_HPP_ */
