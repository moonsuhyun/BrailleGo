
#include <Kernel.hpp>
#include "BspSysTick.h"

Kernel::Kernel() {
	m_is_running = false;
}

void Kernel::Start(void) {
	m_is_running = true;
	TaskManager::sGetInstance().Start();
}

uint32_t Kernel::Create(void (*start_func)(void)) {
	return TaskManager::sGetInstance().TaskCreate(start_func);
}

void Kernel::Yield(void) {
	TaskManager::sGetInstance().TaskYield();
}

void Kernel::Delay(uint32_t ms) {
	TaskManager::sGetInstance().TaskDelay(ms);
}

void Kernel::Terminate(void) {
	TaskManager::sGetInstance().TaskTerminate();
}

uint32_t Kernel::GetSysTick(void) {
	return BSP_Get_Tick();
}

bool Kernel::IsRunning(void) {
	return m_is_running;
}

Kernel& Kernel::sGetInstance(void) {
	static Kernel instance;
	return instance;
}

void Kernel_Start(void) {
	Kernel& kernel = Kernel::sGetInstance();
	kernel.Start();
}

uint32_t Kernel_Create(void (*start_func)(void)) {
	Kernel& kernel = Kernel::sGetInstance();
	return kernel.Create(start_func);
}

void Kernel_Yield(void) {
	Kernel& kernel = Kernel::sGetInstance();
	kernel.Yield();
}

void Kernel_Delay(uint32_t ms) {
	Kernel& kernel = Kernel::sGetInstance();
	kernel.Delay(ms);
}

void Kernel_Terminate() {
	Kernel& kernel = Kernel::sGetInstance();
	kernel.Terminate();
}

uint32_t Kernel_Get_SysTick(void) {
	Kernel& kernel = Kernel::sGetInstance();
	return kernel.GetSysTick();
}

bool Kernel_Is_Running(void) {
	Kernel& kernel = Kernel::sGetInstance();
	return kernel.IsRunning();
}

void Kernel_Init(void) {
	Kernel::sGetInstance();
	TaskManager::sGetInstance();
}
//static volatile bool is_kernel_initialized = false;
//
//void Kernel_Init(void) {
//	if (!is_kernel_initialized) {
//		Kernel_Task_Init();
//	}
//}
//
//void Kernel_Start(void) {
//	if (!is_kernel_initialized) {
//		is_kernel_initialized = true;
//		Kernel_Task_Start();
//	}
//}
//
//uint32_t Kernel_Create(void (*start_func)(void)) {
//	return Kernel_Task_Create(start_func);
//}
//
//void Kernel_Yield(void) {
//	Kernel_Task_Yield(Kernel_Task_Get_Current_Task_Id());
//}
//
//void Kernel_Delay(uint32_t ms) {
//	Kernel_Task_Delay(Kernel_Task_Get_Current_Task_Id(), ms);
//}
//
//void Kernel_Terminate() {
//	Kernel_Task_Terminate(Kernel_Task_Get_Current_Task_Id());
//}
//
//uint32_t Kernel_Get_SysTick(void) {
//	return BSP_Get_Tick();
//}
//
//bool Kernel_Is_Initialized(void) {
//	return is_kernel_initialized;
//}
//
//void Kernel_send_events(uint32_t event_list) {
//    for (uint32_t i=0; i<32; i++) {
//        if ((event_list>>i) & 1) {
//            KernelEventFlag_t sending_event = KernelEventFlag_Empty;
//            sending_event = (KernelEventFlag_t)SET_BIT(sending_event, i);
//            Kernel_event_flag_set(sending_event);
//        }
//    }
//}
//
//KernelEventFlag_t Kernel_wait_events(uint32_t waiting_list) {
//    for (uint32_t i=0; i<32; i++) {
//        if ((waiting_list>>i) & 1) {
//            KernelEventFlag_t waiting_event = KernelEventFlag_Empty;
//            waiting_event = (KernelEventFlag_t)SET_BIT(waiting_event, i);
//            if (Kernel_event_flag_check(waiting_event)) return waiting_event;
//        }
//    }
//    return KernelEventFlag_Empty;
//}
//
//bool Kernel_send_msg(KernelMsgQ_t Qname, void* data, uint32_t count) {
//    uint8_t* d = (uint8_t*) data;
//    for (uint32_t i=0; i<count; i++) {
//        if (false == Kernel_msgQ_enqueue(Qname, *d++)) {
//            for (uint32_t j=0; j<i; j++) {
//                uint8_t rollback;
//                Kernel_msgQ_dequeue(Qname, &rollback);
//            }
//            return false;
//        }
//    }
//    return true;
//}
//
//uint32_t Kernel_recv_msg(KernelMsgQ_t Qname, void* outdata, uint32_t count) {
//    uint8_t* d = (uint8_t*) outdata;
//    for (uint32_t i=0; i<count; i++) {
//        if (!Kernel_msgQ_dequeue(Qname, d++)) return i;
//    }
//    return count;
//}
//
//void Kernel_lock_sem(void) {
//    while (!Kernel_sem_test()) Kernel_Yield();
//}
//
//void Kernel_unlock_sem(void) {
//    Kernel_sem_release();
//}
//
//void Kernel_lock_mutex(void) {
//    while (1) {
//        uint32_t taskId = Kernel_Task_Get_Current_Task_Id();
//        if (!Kernel_mutex_lock(taskId)) Kernel_Yield();
//        else break;
//    }
//}
//void Kernel_unlock_mutex(void) {
//    uint32_t taskId = Kernel_Task_Get_Current_Task_Id();
//    if (!Kernel_mutex_unlock(taskId)) Kernel_Yield();
//}
