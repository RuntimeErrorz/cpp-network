#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <bits/types/sigset_t.h>
#include <sys/wait.h>
#include <sys/syscall.h>

#define gettid() ((pid_t)syscall(SYS_gettid))
void* func(void* arg)
{
    printf("Thread %d %d %ld %d\n",getpid(),getppid(),pthread_self(),gettid());
    return NULL;
}
int main()
{
    pid_t pid = getpid();
    pid_t p = fork();
    if (!p)
    {
        printf("Child %d %d %ld %d\n",getpid(),getppid(),pthread_self(),gettid());
    }
    else
    {
        printf("Father %d %d %ld %d\n",getpid(),getppid(),pthread_self(),gettid());
        sleep(1);
        pthread_t thread_handles;
        pthread_create(&thread_handles, NULL, func, NULL);
    }
}