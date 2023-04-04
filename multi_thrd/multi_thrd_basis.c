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

#define gettid() (syscall(SYS_gettid))
#define LENGTH 310
int conc_amnt;

struct member
{
    int flag;
    char s[LENGTH];
};

void *thrdfunc(void *arg)
{
    pthread_t ptid = pthread_self();
    struct member *temp = (struct member *)arg;
    if (temp->flag)
        pthread_detach(ptid);
    pid_t pid = getpid();
    long tid = gettid();
    printf("[spid](%ld) [tgid](%d) [ptid](%lu) start\n", tid, pid, ptid);
    printf("[spid](%ld) [read](%s)\n", tid, temp->s);
    printf("[spid](%ld) [tgid](%d) [ptid](%lu) [func](pthread_exit)\n", tid, pid, ptid);
    pthread_exit(NULL);
    return NULL;
}

void replaceLineFeed(char *s)
{
    int i;
    for (i = 0; s[i] != '\n'; i++)
        ;
    s[i] = '\0';
}
int main(int argc, char **argv)
{
    pid_t pid = getpid();
    pthread_t ptid = pthread_self();
    printf("[spid](%ld) [tgid](%d) [ptid](%lu) start\n", gettid(), pid, ptid);
    conc_amnt = atoi(argv[2]) - 1;
    FILE *fp = fopen("td.txt", "r");
    char *buf = (char *)malloc(LENGTH);
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_t *thread_handles = (pthread_t *)malloc(conc_amnt * sizeof(pthread_t));

    struct member temps[conc_amnt];
    for (int i = 0; i < conc_amnt; i++)
    {
        fgets(buf, LENGTH, fp);
        replaceLineFeed(buf);
        printf("[spid](%ld) [read](%s)\n", gettid(), buf);
        struct member temp;
        temp.flag = i;
        strcpy(temp.s, buf);
        temps[i] = temp;
    }

    for (int i = 0; i < conc_amnt; i++)
    {
        if (!i)
        {
            pthread_create(&thread_handles[i], &attr, thrdfunc, (void *)&temps[i]);
            pthread_attr_destroy(&attr);
        }
        else
            pthread_create(&thread_handles[i], NULL, thrdfunc, (void *)&temps[i]);
    }
    fclose(fp);
    sleep(1);
    printf("[spid](%ld) [tgid](%d) [ptid](%lu) [func](exit)\n", gettid(), pid, ptid);
    exit(0);
}