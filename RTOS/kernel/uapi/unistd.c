//
// Created by 문수현 on 25. 10. 19..
//

#include <sys/unistd.h>

unsigned sleep(unsigned int ms)
{

}

pid_t getpid(void)
{

}

// exit -> stdlib.h
void _exit(int status)
{
    while (1)
    {

    }
}

int read(int fd, void *buf, size_t nbyte)
{

}

int write(int fd, const void *buf, size_t nbyte)
{

}

int close(int fd)
{

}

int isatty(int fd)
{

}

int unlink(const char *path)
{

}

int link(const char* oldpath, const char* newpath)
{

}

pid_t fork(void)
{

}

int execve(const char* path, char* const argv[], char* const envp[])
{

}