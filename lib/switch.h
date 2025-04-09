#ifndef LIB_SWITCH_H_
#define LIB_SWITCH_H_

// naked: stack, return address 등 함수 호출 관련 어셈블리가 생성되지 않음
__attribute__ ((naked)) void Kernel_task_context_switching(void);
__attribute__ ((naked)) void Save_context(void);
__attribute__ ((naked)) void Restore_context(void);

#endif // LIB_SWITCH_H_