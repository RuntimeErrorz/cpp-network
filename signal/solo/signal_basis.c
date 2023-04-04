#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <bits/types/sigset_t.h>
void sig_handler(int signum)
{
    pid_t pid = getpid();
    if (signum == SIGUSR1)
    {
        printf("[pid](%d): [sig](SIGUSR1) is captured\n", pid);
    }
    else if (signum == SIGUSR2)
    {
        printf("[pid](%d): [sig](SIGUSR2) is captured\n", pid);
    }
}
void sig_int(int signum)
{
    exit(0);
}
int main()
{
    struct sigaction act;
    memset(&act, 0, sizeof(act));
    sigemptyset(&act.sa_mask);
    act.sa_handler = sig_handler;

    struct sigaction act2;
    memset(&act2, 0, sizeof(act2));
    sigemptyset(&act2.sa_mask);
    act2.sa_handler = sig_int;
    sigaction(SIGINT, &act2, NULL);
    sigaction(SIGUSR1, &act, NULL);
    sigaction(SIGUSR2, &act, NULL);
    pid_t cpid = getpid();
    printf("[pid](%d): start\n", cpid);
    while (1)
    {
        pause();
    }
}