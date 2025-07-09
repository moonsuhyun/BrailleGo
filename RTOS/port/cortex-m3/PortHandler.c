/*
 * PortHandler.c
 *
 *  Created on: May 26, 2025
 *      Author: moon
 */


#include <Kernel.h>
#include "PortHandler.h"

#include <stdio.h>
#include <stdbool.h>
#include <TaskManager.h>

#include "types.h"
#include "devio.h"
#include "stm32f1xx.h"

extern UART_HandleTypeDef huart2;
static KernelTcb_t* current_tcb;


void Port_Handler_HardFault(uint32_t* sp) {
	uint32_t pc = sp[6];
	printf("pc=%x", pc);
	while (1) {

	}
}

__attribute ((naked)) void Port_Handler_SVC(void) {
	asm volatile ("PUSH  {r7, lr}\n");
	current_tcb = Kernel_Task_Get_Current_Task();
	asm volatile ("POP   {r7, lr}\n");
    asm volatile (
        // SVC를 호출한 스택프레임 선택
        "TST   lr, #4              \n"	// EXC_RETURN의 2번 비트로 PSP/MSP 선택
        "ITE   eq                  \n"
        "MRSEQ r0, msp             \n"
        "MRSNE r0, psp             \n"
        // SVC 번호 추출
        "LDR   r1, [r0, #24]       \n"	// r1 <- stacked PC (SVC 이후 명령)
        "SUBS  r1, #2              \n"	// r1 = SVC 명령 위치 (PC - 2)
        "LDRB  r2, [r1]            \n"	// r2 = SVC #번호 (SVC 명령의 LSB)
        // SVC 번호에 따른 분기
        "CMP   r2, #0              \n"  // SVC#0 = 태스크 시작
        "BEQ   svc_task_start 	   \n"
    	"CMP   r2, #1			   \n"  // SVC#1 = 컨택스트 스위칭을 위해 PendSV 발생
    	"BEQ   svc_trigger_pendsv  \n"
        "BX    lr                  \n"	// (그 외 번호: 그냥 예외 복귀)

		// 태스크 시작 컨텍스트 복구/Thread Mode PSP 전환
    	"svc_task_start:           \n"
		"LDR   r0, =current_tcb   \n"
		"LDR   r0, [r0]        	   \n"
		"LDR   r0, [r0]            \n"  // r0 = gCurrent_tcb->sp
		"LDMIA r0!, {r4-r11}       \n"  // 태스크 스택 프레임 복구
		"MSR   psp, r0             \n"  // PSP 복구
		// Control 레지스터를 2(PSP사용, Privileged)로 변경
		"MOVS  r0, #2              \n"
		"MSR   control, r0         \n"
		"ISB                       \n"  // Instruction Sync. Barrier
		// EXC_RETURN 값으로 복귀(PSP활성, Thread Mode 복귀)
		"MOV   lr, #0xFFFFFFFD     \n"
		"BX    lr                  \n"
    	// 컨택스트 스위칭을 처리하기 위해 PendSV 발생
		"svc_trigger_pendsv:       \n"
		"LDR   r0, =0xE000ED04     \n"  // SCB->ICSR 주소
		"LDR   r1, [r0]            \n"
		"ORR   r1, r1, #0x10000000 \n"  // 28번 bit(PENDSVSET) set
		"STR   r1, [r0]            \n"  // ICSR에 저장해서 PendSV 발생
		"BX    lr                  \n"
    );
}

__attribute ((naked)) void Port_Handler_PendSV(void) {
	asm volatile ("PUSH  {r7, lr}\n");
	current_tcb = Kernel_Task_Get_Current_Task();
	asm volatile ("POP   {r7, lr}\n");
	asm volatile (
		// 현재 태스크 컨텍스트 백업
		"MRS   r0, psp             \n"
		"STMDB r0!, {r4-r11}       \n"  // 현재 태스크 스택 프레임 백업
		"LDR   r1, =current_tcb    \n"
		"LDR   r1, [r1]            \n"  // r1 = Current_tcp->sp
		"STR   r0, [r1]            \n"  // PSP 백업
	);
	asm volatile ("PUSH  {r7, lr}\n");
	Kernel_Task_Scheduler();
	current_tcb = Kernel_Task_Get_Current_Task();
	asm volatile ("POP   {r7, lr}\n");
	asm volatile (
		// 다음 태스크 컨택스트 복원
		"LDR   r1, =current_tcb    \n"
		"LDR   r1, [r1]            \n"
		"LDR   r0, [r1]            \n"  // r0 = Next_tcb->sp
		"LDMIA r0!, {r4-r11}       \n"  // 다음 태스크 스택 프레임 복구
		"MSR   psp, r0             \n"  // PSP 복구
		"BX    lr                  \n"
	);
}

void Port_Handler_SysTick(void) {
	if (Kernel_Is_Running()) {
		Kernel_Task_SysTick_Callback();
	}
}

