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

int sig_to_exit = 0;
int sig_type = 0;
FILE *fp;
int listenfd = 0;

void sig_int(int signo)
{
    sig_type = signo;
    sig_to_exit = 1;
    fprintf(fp, "[srv](%d) SIGINT is coming!\n", getpid());
}

void sig_pipe(int signo)
{
    sig_type = signo;
    fprintf(fp, "[srv](%d) SIGPIPE is coming!\n", getpid());
}

void sig_chld(int signo)
{
    sig_type = signo;
    fprintf(fp, "[srv](%d) SIGCHLD is coming!\n", getpid());

    pid_t pid_chld;
    while ((pid_chld = waitpid(-1, NULL, WNOHANG)) > 0)
        ;
}

int loop_read(int sockfd, char *buf, int len)
{
    int offset = 0;
    while (1)
    {
        int res = read(sockfd, buf + offset, len);
        if (res < 0) // On error, -1 is returned
        {
            printf("[srv] read data return %d and errno is %d\n", res, errno);
            if (errno == EINTR)
            {
                if (sig_type == SIGINT)
                {
                    return 1; //但凡收到SIGINT，指示服务器结束
                }
                continue; //若是其他信号中断，则重新执行读取
            }
            return 1;
        }
        else if (res == 0) //(zero indicates end of file)
            return 0;
        else if (res < len)
        {
            len -= res;
            offset += res;
        }
        else // res == len_h
            return 2;
    }
}
int echo_rep(int sockfd, FILE *fp_res)
{
    char buf[MAX_CMD_STR + 1];
    int pin, length;
    while (1)
    {
        memset(buf, 0, sizeof(buf));
        if (loop_read(sockfd, buf,  4) != 2)
            return pin;
        memcpy(&pin, buf, 4);
        pin = ntohl(pin);
        if (loop_read(sockfd, buf + 4, 4) != 2)
            return pin;
        memcpy(&length, buf + 4, 4);
        length = ntohl(length);
        if (loop_read(sockfd, buf + 8, length) != 2)
            return pin;
        fprintf(fp_res, "[echo_rqt](%d) %s\n", getpid(), buf + 8);
        write(sockfd, buf, length + 8);
    }
}
void tcp_work(int connfd)
{
    char fn_res[20] = {0};
    sprintf(fn_res, "stu_srv_res_%d.txt", getpid());
    FILE *fp_res = fopen(fn_res, "wb");
    printf("[srv](%d) stu_srv_res_%d.txt is opened!\n", getpid(), getpid());
    fprintf(fp_res, "[srv](%d) child process is created!\n", getpid());
    close(listenfd);
    fprintf(fp_res, "[srv](%d) listenfd is closed!\n", getpid());
    int pin = echo_rep(connfd, fp_res);
    char fn_res_new[20] = {0};
    sprintf(fn_res_new, "stu_srv_res_%d.txt", pin);
    rename(fn_res, fn_res_new);
    fprintf(fp_res, "[srv](%d) res file rename done!\n", getpid());
    close(connfd);
    fprintf(fp_res, "[srv](%d) connfd is closed!\n", getpid());
    fprintf(fp_res, "[srv](%d) child process is going to exit!\n", getpid());
    printf("[srv](%d) stu_srv_res_%d.txt is closed!\n", getpid(), getpid());
    fclose(fp_res);
    exit(0);
}

// 业务逻辑处理函数；

int main(int argc, char **argv)
{
    fp = fopen("stu_srv_res_p.txt", "wb");
    printf("[srv](%d) stu_srv_res_p.txt is opened\n", getpid());

    struct sigaction sigact_pipe, old_sigact_pipe;
    sigact_pipe.sa_handler = sig_pipe;
    sigemptyset(&sigact_pipe.sa_mask);
    sigact_pipe.sa_flags = 0;
    sigact_pipe.sa_flags |= SA_RESTART; //通过SA_RESTART设置受影响的慢调用在中断结束后立刻重启：
    sigaction(SIGPIPE, &sigact_pipe, &old_sigact_pipe);

    struct sigaction sigact_chld, old_sigact_chld;
    sigact_chld.sa_handler = sig_chld;
    sigemptyset(&sigact_chld.sa_mask);
    sigact_chld.sa_flags = 0;
    sigact_chld.sa_flags |= SA_RESTART; //通过SA_RESTART设置受影响的慢调用在中断结束后立刻重启：
    sigaction(SIGCHLD, &sigact_chld, &old_sigact_chld);

    struct sigaction sigact_int, old_sigact_int;
    sigact_int.sa_handler = sig_int;
    sigemptyset(&sigact_int.sa_mask);
    sigact_int.sa_flags = 0;
    sigaction(SIGINT, &sigact_int, &old_sigact_int);

    struct sockaddr_in srv_addr, cli_addr;
    bzero(&srv_addr, sizeof(srv_addr));
    bzero(&cli_addr, sizeof(cli_addr));
    socklen_t cli_addr_len = sizeof(cli_addr);
    int connfd;

    srv_addr.sin_family = AF_INET;
    inet_pton(AF_INET, argv[1], &srv_addr.sin_addr);
    srv_addr.sin_port = (u_int16_t)atoi(argv[2]);
    fprintf(fp, "[srv] server[%s:%hu] is initializing!\n", argv[1], srv_addr.sin_port);
    fflush(fp);
    srv_addr.sin_port = htons(srv_addr.sin_port);

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    int res = bind(listenfd, (struct sockaddr *)&srv_addr, sizeof(srv_addr));
    res = listen(listenfd, 100);

    while (!sig_to_exit)
    {
        connfd = accept(listenfd, (struct sockaddr *)&cli_addr, (socklen_t *)&cli_addr_len);

        if (connfd == -1 && errno == EINTR && sig_type == SIGINT) // connfd -1 for errors
        {
            break;
        }
        char cli_ip_addr[MAX_IP_LEN + 1];
        inet_ntop(AF_INET, &cli_addr.sin_addr, cli_ip_addr, sizeof(cli_ip_addr));
        fprintf(fp, "[srv] client[%s:%hu] is accepted!\n", cli_ip_addr, ntohs(cli_addr.sin_port));
        fflush(fp);

        pid_t pid = fork();
        if (!pid)
        {
            tcp_work(connfd);
        }
        close(connfd);
    }
    close(listenfd);
    fprintf(fp, "[srv](%d) listenfd is closed!\n", getpid());
    fprintf(fp, "[srv](%d) parent process is going to exit!\n", getpid());
    fclose(fp);
    printf("[srv] (%d) stu_srv_res_p.txt is closed!\n", getpid());
    return 0;
}
// tar cvf src.tar tcp_echo_cli.c tcp_echo_srv.c