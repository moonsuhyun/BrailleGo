/*
 * svc.h
 *
 *  Created on: May 26, 2025
 *      Author: moon
 */

// SVC(Supervised Call) 관련
#ifndef KERNEL_SVC_H_
#define KERNEL_SVC_H_

typedef enum KernelSvcNo_t {
	KernelSvcNo_TaskStart,
	KernelSvcNo_ContextSwitch

} KernelSvcNo_t;


#endif /* KERNEL_SVC_H_ */
