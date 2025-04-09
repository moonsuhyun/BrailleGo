#include "stdint.h"

#include "switch.h"
#include "task.h"

extern KernelTcb_t Current_tcb;
extern KernelTcb_t Next_tcb;

__attribute__ ((naked)) void Kernel_task_context_switching(void) {
    __asm__ ("B Save_context");
    __asm__ ("B Restore_context");
}

__attribute__ ((naked)) void Save_context(void) {
    // 현재 task의 context를 stack에 push
    __asm__ ("PUSH {lr}");  // Context switching 호출 이후 return address
    __asm__ ("PUSH {r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12}");
    __asm__ ("MRS r0, cpsr");   // psr to register
    __asm__ ("PUSH {r0}");
    // 현재 task의 stack pointer(방금 백업 된 context를 pointing)를 Current_tcb에 저장
    __asm__ ("LDR r0, =Current_tcb");
    __asm__ ("LDR r0, [r0]");
    __asm__ ("STR sp, [r0]");
}

__attribute__ ((naked)) void Restore_context(void) {
    // 다음에 실행될 task의 stack pointer 복구
    __asm__ ("LDR r0, =Next_tcb");
    __asm__ ("LDR r0, [r0]");
    __asm__ ("LDR sp, [r0]");
    // stack에서 다음 task의 context pop
    __asm__ ("POP {r0}");
    __asm__ ("MSR cpsr, r0");
    __asm__ ("POP {r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12}");
    __asm__ ("POP {pc}");
}