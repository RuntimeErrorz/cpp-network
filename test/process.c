#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <string.h>
#include <bits/types/sigset_t.h>
#include <sys/wait.h>
#include <sys/syscall.h>

int main()
{
    pid_t pid = fork();
    if (!pid) // child proc 1
    {
        printf("Child1's pid: %d\n", getpid());
        printf("Child1's ppid: %d\n", getppid());
        printf("Child1's pgid: %d\n", getpgrp());
        printf("Child1's spid: %ld\n", syscall(SYS_gettid));
        printf("Child1's ptid: %lu\n", pthread_self());
        exit(0);
    }
    else
    {
        sleep(1);
        pid_t pid = fork();
        if (!pid) // child proc 1
        {
            printf("Child2's pid: %d\n", getpid());
            printf("Child2's ppid: %d\n", getppid());
            printf("Child2's pgid: %d\n", getpgrp());
            printf("Child2's spid: %ld\n", syscall(SYS_gettid));
            printf("Child2's ptid: %lu\n", pthread_self());
            exit(0);
        }
        else
        {
            sleep(1);
            printf("Father's pid: %d\n", getpid());
            printf("Father's ppid: %d\n", getppid());
            printf("Father's pgid: %d\n", getpgrp());
            printf("Father's spid: %ld\n", syscall(SYS_gettid));
            printf("Father's ptid: %lu\n", pthread_self());
            exit(0);
        }
    }
}