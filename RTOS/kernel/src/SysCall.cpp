// //
// // Created by 문수현 on 25. 10. 18..
// //
//
// #include "SysCall.h"
// #include "TaskManager.hpp"
// #include "Task.hpp"
//
// inline int is_valid_signal(int sig)
// {
//     return sig>0 && sig<32;
// }
// inline int is_uncatchable(int sig)
// {
//     return sig==SIGKILL || sig==SIGSTOP;
// }
//
// typedef enum { ACT_TERM, ACT_IGN, ACT_STOP, ACT_CONT } DefaultAction_t;
//
// inline DefaultAction_t default_action(int sig)
// {
//     switch(sig)
//     {
//     case SIGKILL: return ACT_TERM;
//     case SIGSTOP: return ACT_STOP;
//     case SIGCONT: return ACT_CONT;
//     case SIGSEGV:
//     case SIGILL:
//     case SIGABRT:
//         return ACT_TERM;
//     default: return ACT_TERM;
//     }
// }
//
// inline uint32_t sigbit(int sig)
// {
//     return 1u << (sig & 31);
// }
//
// inline int pick_next_signal(sigset_t pend, sigset_t mask)
// {
//     uint32_t avail = pend & ~mask;
//     if (!avail) return 0;
// #if defined(__GNUC__)
//     return __builtin_ctz(avail);
// #else
//     for (int i=0;i<32;i++) if (avail & (1u<<i)) return i;
//     return 0;
// #endif
// }
//
// int SYS_getpid(void)
// {
//     TaskManager tm = TaskManager::sGetInstance();
//     return static_cast<int> (tm.GetRunningTaskId());
// }
//
// int SYS_kill(int pid, int sig)
// {
//     if (!is_valid_signal(sig)) return -EINVAL;
//     TaskManager tm = TaskManager::sGetInstance();
//     return tm.PendSignalById(pid, sig);
//
//     // request_reschedule();
// }
//
// int SYS_sigaction(int sig, const struct sigaction* act, struct sigaction* old)
// {
//     if (!is_valid_signal(sig)) return -EINVAL;
//     if (act && is_uncatchable(sig)) return -EINVAL;
//
//     TaskManager tm = TaskManager::sGetInstance();
//     Task* task = tm.GetRunningTask();
//
//     if (old) *old = task->GetSigAction(sig);
//     if (act) task->SetSigAction(sig, *act);
//
//     return 0;
// }
//
// int SYS_sigprocmask(int how, const sigset_t* set, sigset_t* old)
// {
//     TaskManager tm = TaskManager::sGetInstance();
//     Task* task = tm.GetRunningTask();
//
//     if (old) *old = task->GetSigMask();
//     if (!set) return 0;
//
//     switch (how)
//     {
//     case SIG_BLOCK:
//         task->SetSigBlock(*set);
//         break;
//     case SIG_UNBLOCK:
//         task->SetSigUnblock(*set);
//         break;
//     case SIG_SETMASK:
//         task->SetSigMask(*set);
//         break;
//     default:
//         return -EINVAL;
//     }
//
//     // SIGKILL/SIGSTOP은 마스크 불가
//     task->SetSigUnblock(1u<<SIGKILL|1u<<SIGSTOP);
//     return 0;
// }
//
// int SYS_sigpending(sigset_t* out)
// {
//     if (!out) return -EINVAL;
//
//     TaskManager tm = TaskManager::sGetInstance();
//     Task* task = tm.GetRunningTask();
//
//     *out = task->GetSigPending();
//
//     return 0;
// }
//
// int SYS_sigreturn(void)
// {
//     TaskManager tm = TaskManager::sGetInstance();
//     Task* task = tm.GetRunningTask();
//     task->SigReturn();
//
//     return 0;
// }
//
// int SYS_exit(int status)
// {
//     // task_terminate_with_status(cur, status);
//
//     TaskManager tm = TaskManager::sGetInstance();
//     tm.TaskTerminate();
//     for(;;){} /* noreturn */
//
//
// }
