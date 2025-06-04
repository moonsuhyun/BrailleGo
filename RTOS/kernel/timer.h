/*
 * timer.h
 *
 *  Created on: Jun 2, 2025
 *      Author: moon
 */

#ifndef KERNEL_TIMER_H_
#define KERNEL_TIMER_H_

void Kernel_timer_init(void);
__attribute ((naked)) void Kernel_timer_systick_handler(void);

#endif /* KERNEL_TIMER_H_ */
