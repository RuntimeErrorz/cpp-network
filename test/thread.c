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

void *print(void *s)
{
    s = (char *)s;
    printf("%s's pid: %d\n", s, getpid());
    printf("%s's ppid: %d\n", s, getppid());
    printf("%s's pgid: %d\n", s, getpgrp());
    printf("%s's spid: %ld\n", s, syscall(SYS_gettid));
    printf("%s's ptid: %lu\n", s, pthread_self());
}
void print2(char *s)
{
    printf("%s's pid: %d\n", s, getpid());
    printf("%s's ppid: %d\n", s, getppid());
    printf("%s's pgid: %d\n", s, getpgrp());
    printf("%s's spid: %ld\n", s, syscall(SYS_gettid));
    printf("%s's ptid: %lu\n", s, pthread_self());
}
int main()
{
    pthread_t child[2];

    pthread_create(&child[0], NULL, print, "Child1");
    sleep(1);
    pthread_create(&child[0], NULL, print, "Child2");
    sleep(1);
    print2("Father");
}