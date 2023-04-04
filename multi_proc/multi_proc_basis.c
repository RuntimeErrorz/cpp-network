#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char **argv)
{
    char buffer[123] = {0};
    pid_t cpid = getpid();
    pid_t ppid = getppid();
    printf("[pid](%d) [ppid](%d): start\n", cpid, ppid);
    int fd1_td1 = open("td1.txt", O_RDONLY);
    printf("[pid](%d) [ppid](%d): [fd1_td1](%d) is got\n", cpid, ppid, fd1_td1);
    for (int i = 0; i < atoi(argv[2]) - 1; ++i)
    {
        pid_t pid = fork();
        if (!pid) // child processes
        {
            cpid = getpid();
            ppid = getppid();
            printf("[pid](%d) [ppid](%d): start\n", cpid, ppid);
            memset(buffer, '\0', sizeof(buffer));
            read(fd1_td1, buffer, 5);
            printf("[pid](%d) [ppid](%d): [read](%s) by [fd1_td1](%d)\n", cpid, ppid, buffer, fd1_td1);
            int fd2_td1 = open("td1.txt", O_RDONLY);
            printf("[pid](%d) [ppid](%d): [fd2_td1](%d) is got\n", cpid, ppid, fd2_td1);
            memset(buffer, '\0', sizeof(buffer));
            read(fd2_td1, buffer, 5);
            printf("[pid](%d) [ppid](%d): [read](%s) by [fd2_td1](%d)\n", cpid, ppid, buffer, fd2_td1);
            close(fd2_td1);
            close(fd1_td1);
            printf("[pid](%d) [ppid](%d): exit\n", cpid, ppid);
            exit(0);
        }
    }
    memset(buffer, '\0', sizeof(buffer));
    read(fd1_td1, buffer, 5);
    printf("[pid](%d) [ppid](%d): [read](%s) by [fd1_td1](%d)\n", cpid, ppid, buffer, fd1_td1);
    close(fd1_td1);
    int fd1_td2 = open("td2.txt", O_RDONLY);
    printf("[pid](%d) [ppid](%d): [fd1_td2](%d) is got\n", cpid, ppid, fd1_td2);
    memset(buffer, '\0', sizeof(buffer));
    read(fd1_td2, buffer, 5);
    printf("[pid](%d) [ppid](%d): [read](%s) by [fd1_td2](%d)\n", cpid, ppid, buffer, fd1_td2);
    close(fd1_td2);
    sleep(10);
    printf("[pid](%d) [ppid](%d): exit\n", cpid, ppid);
    exit(0);
}