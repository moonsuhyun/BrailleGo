//
// Created by 문수현 on 25. 10. 18..
//

#include <sys/signal.h>
#include <errno.h>

#include "syscall_no.h"
#include "PortCore.h"

int svc_call3(int sysno, int a0, int a1, int a2);

int getpid(void){
    return Port_Core_SVC_Call(SYS_GETPID,0,0,0);
}

int kill(int pid, int sig){
    int rc = Port_Core_SVC_Call(SYS_KILL, pid, sig, 0);
    if (rc < 0) { errno = -rc; return -1; }
    return 0;
}

int raise(int sig){
    return kill(getpid(), sig);
}

int sigaction(int sig, const struct sigaction* act, struct sigaction* old){
    int rc = Port_Core_SVC_Call(SYS_SIGACTION, sig, (int)act, (int)old);
    if (rc < 0) { errno = -rc; return -1; }
    return 0;
}

int sigprocmask(int how, const sigset_t* set, sigset_t* old){
    int rc = Port_Core_SVC_Call(SYS_SIGPROCMASK, how, (int)set, (int)old);
    if (rc < 0) { errno = -rc; return -1; }
    return 0;
}

int sigpending(sigset_t* out){
    int rc = Port_Core_SVC_Call(SYS_SIGPENDING, (int)out, 0, 0);
    if (rc < 0) { errno = -rc; return -1; }
    return 0;
}


__attribute ((noreturn))
void sig_trampoline(void)
{
    (void) Port_Core_SVC_Call(SYS_SIGRETURN, 0, 0, 0);
    while (1);
}