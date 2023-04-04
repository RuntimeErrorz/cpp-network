#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <bits/types/sigset_t.h>
#include <sys/wait.h>
int conc_amnt = 4;
void pa_handle_sigint()
{
    pid_t pid = getpid();
    printf("[pid](%d): SIGINT is handled in pa_handle_sigint()\n", pid);
    printf("[pid](%d): [func](exit)  with [conc_amnt](%d)\n", pid, conc_amnt);
    exit(0);
}
void pa_handle_sigchld()
{
    pid_t cpid;
    pid_t pid = getpid();
    printf("[pid](%d): SIGCHLD is handled in pa_handle_sigchld()\n", pid);
    while ((cpid = waitpid(-1, NULL, WNOHANG)) > 0)
    {
        printf("[pid](%d): [cpid](%d) is waited in pa_handle_sigchld()\n", pid, cpid);
        conc_amnt--;
    }
}
void sig_handle(int signum)
{
    if (signum == SIGINT)
        pa_handle_sigint();
    else
        pa_handle_sigchld();
}
int main(int argc, char **argv)
{
    struct sigaction act;
    memset(&act, 0, sizeof(act));
    sigemptyset(&act.sa_mask);
    act.sa_handler = sig_handle;
    sigaction(SIGINT, &act, NULL);
    sigaction(SIGCHLD, &act, NULL);
    pid_t pids[conc_amnt - 1];
    for (int i = 0; i < conc_amnt - 1; i++)
    {
        pid_t pid = fork();
        if (!pid)
        {
            pid_t cpid = getpid();
            printf("[pid](%d): start\n", cpid);
            pause();
        }
        else
            pids[i] = pid;
    }
    while (conc_amnt > 1)
    {
        printf("[pid](%d): [func](pause) with [conc_amnt](%d)\n", getpid(), conc_amnt);
        pause();
    }
    pause();
}