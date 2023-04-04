#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h>
#define MAX_IP_LEN 19
#define MAX_CMD_STR 108

void sig_chld(int signo)
{
    pid_t pid_chld;
    while ((pid_chld = waitpid(-1, NULL, WNOHANG)) > 0)
        ;
}

int loop_read(int sockfd, char *buf, int len) //修改此函数
{
    int offset = 0;
    while (1)
    {
        int res = read(sockfd, buf + offset, len);
        if (res < 0) // On error, -1 is returned
        {
            printf("[cli] read data return %d and errno is %d\n", res, errno);
            return 1;
        }
        else if (res == 0) //(zero indicates end of file)
            return 0;
        else if (res < len)
        {
            len -= res;
            offset += res;
            printf("[cli] read more\n");
        }
        else
        {
            return 2;
        } // res == len
    }
}

int echo_rqt(int sockfd, int pin, FILE *res)
{
    char filename[MAX_CMD_STR + 1] = {0};
    sprintf(filename, "td%d.txt", pin);
    FILE *src = fopen(filename, "r");

    char buf[MAX_CMD_STR + 1];
    memset(buf, 0, sizeof(buf));
    while (fgets(buf, MAX_CMD_STR, src))
    {
        if (strncmp(buf, "exit", 4) == 0)
            return 0;
        int len_h = strnlen(buf, MAX_CMD_STR);
        if (buf[len_h - 1] == '\n')
            buf[len_h - 1] = 0;
        int len_n = htonl(len_h), pin_n = htonl(pin);
        write(sockfd, &pin_n, sizeof(pin_n));
        write(sockfd, &len_n, sizeof(len_n));
        write(sockfd, buf, len_h);
        memset(buf, 0, sizeof(buf));
        read(sockfd, &pin_n, sizeof(pin_n));
        read(sockfd, &len_n, sizeof(len_n));
        len_h = ntohl(len_n);
        if (loop_read(sockfd, buf, len_h) != 2)
        {
            printf("[cli] read data error\n");
            return 1;
        }
        fprintf(res, "[echo_rep](%d) %s\n", getpid(), buf);
        memset(buf, 0, sizeof(buf));
    }
    return 0;
}
void tcp_work(struct sockaddr_in srv_addr, int index)
{
    char filename[MAX_CMD_STR] = {0};
    sprintf(filename, "stu_cli_res_%d.txt", index);
    FILE *fp = fopen(filename, "wb");
    printf("[cli](%d) stu_cli_res_%d.txt is created\n", getpid(), index);

    if (index)
        fprintf(fp, "[cli](%d) child process %d is created!\n", getpid(), index);

    int connfd = socket(AF_INET, SOCK_STREAM, 0);
    int res = connect(connfd, (struct sockaddr *)&srv_addr, sizeof(srv_addr));
    if (res == 0)
    {
        char srv_ip_addr[MAX_IP_LEN] = {0};
        inet_ntop(AF_INET, &srv_addr.sin_addr, srv_ip_addr, sizeof(srv_ip_addr));
        fprintf(fp, "[cli](%d) server[%s:%hu] is connected!\n", getpid(), srv_ip_addr, ntohs(srv_addr.sin_port));
        echo_rqt(connfd, index, fp);
    }
    else if (res == -1)
        printf("[cli] connect error! errno is %d\n", errno);
    close(connfd);
    fprintf(fp, "[cli](%d) connfd is closed!\n", getpid());
    if (index)
        fprintf(fp, "[cli](%d) child process is going to exit!\n", getpid());
    else
        fprintf(fp, "[cli](%d) parent process is going to exit!\n", getpid());
    fclose(fp);
    printf("[cli](%d) stu_cli_res_%d.txt is closed!\n", getpid(), index);
    if (!index)
        while (wait(NULL) > 0)
            ;
    else
        exit(0);
}

int main(int argc, char **argv)
{
    struct sigaction sigact_chld, old_sigact_chld;
    sigact_chld.sa_handler = sig_chld;
    sigemptyset(&sigact_chld.sa_mask);
    sigact_chld.sa_flags |= SA_RESTART; //通过SA_RESTART设置受影响的慢调用在中断结束后立刻重启：
    sigact_chld.sa_flags = 0;

    sigaction(SIGCHLD, &sigact_chld, &old_sigact_chld);

    int conc_amnt = atoi(argv[3]);

    struct sockaddr_in srv_addr;
    bzero(&srv_addr, sizeof(srv_addr));

    srv_addr.sin_family = AF_INET;
    inet_pton(AF_INET, argv[1], &srv_addr.sin_addr);
    srv_addr.sin_port = htons((u_int16_t)atoi(argv[2]));

    for (size_t i = 1; i <= conc_amnt - 1; i++)
    {
        pid_t pid = fork();
        if (!pid)
            tcp_work(srv_addr, i);
    }
    tcp_work(srv_addr, 0);
    return 0;
}