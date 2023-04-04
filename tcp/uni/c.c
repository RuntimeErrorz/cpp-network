#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#define MAX_IP_LEN 19
#define MAX_CMD_STR 108
int loop_read(int sockfd, char *buf, int len)
{
    int offset = 0;
    while (1)
    {
        int res = read(sockfd, buf + offset, len);
        if (res < 0) // On error, -1 is returned
        {
            printf("[srv] read data return %d and errno is %d\n", res, errno);
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
int makePDU(char *s, int pin)
{
    pin = htonl(pin);
    int i;
    for (i = 0; s[i] != '\n'; i++)
        ;
    s[i] = '\0';
    int len = htonl(i + 1);
    for (int j = 8; j < i + 8; j++)
        s[j] = s[j - 8];
    memset(s, 0, 8);
    memcpy(s, &pin, 4);
    memcpy(s + 4, &len, 4);
    printf("send %d %d\n", ntohl(pin), ntohl(len));
    return ntohl(len);
}
int echo_rqt(int sockfd, int pin)
{

    char buf[MAX_CMD_STR + 1];
    memset(buf, 0, sizeof(buf));
    while (fgets(buf, MAX_CMD_STR, stdin))
    {
        if (strncmp(buf, "exit", 4) == 0)
            return 0;
        int length = makePDU(buf, pin);
        write(sockfd, buf, length + 8);
        memset(buf, 0, sizeof(buf));
        if (loop_read(sockfd, buf, 4) != 2)
        {
            printf("[cli] read pin error\n");
            return 1;
        }
        memcpy(&pin, buf, 4);
        pin = ntohl(pin);
        if (loop_read(sockfd, buf + 4, 4) != 2)
        {
            printf("[cli] read length error\n");
            return 1;
        }
        memcpy(&length, buf + 4, 4);
        length = ntohl(length);
        printf("read %d %d\n", pin, length);
        if (loop_read(sockfd, buf + 8, length) != 2)
        {
            printf("[cli] read data error\n");
            return 1;
        }
        printf("[echo_rep] (%d) %s\n", getpid(), buf + 8);
        memset(buf, 0, sizeof(buf));
    }
    return 0;
}

void tcp_work(struct sockaddr_in srv_addr, int index)
{

    int connfd = socket(AF_INET, SOCK_STREAM, 0);
    int res = connect(connfd, (struct sockaddr *)&srv_addr, sizeof(srv_addr));
    if (res == 0)
    {
        char srv_ip_addr[MAX_IP_LEN] = {0};
        inet_ntop(AF_INET, &srv_addr.sin_addr, srv_ip_addr, sizeof(srv_ip_addr));
        printf("[cli] server[%s:%hu] is connected!\n", srv_ip_addr, ntohs(srv_addr.sin_port));
        echo_rqt(connfd, index);
    }
    else if (res == -1)
        printf("[cli] connect error! errno is %d\n", errno);
    close(connfd);
    printf("[cli] (%d) connfd is closed!\n", getpid());
    printf("[cli] (%d) parent process is going to exit!\n", getpid());
    exit(0);
}

int main(int argc, char **argv)
{

    struct sockaddr_in srv_addr;
    bzero(&srv_addr, sizeof(srv_addr));

    srv_addr.sin_family = AF_INET;
    inet_pton(AF_INET, argv[1], &srv_addr.sin_addr);
    srv_addr.sin_port = htons((u_int16_t)atoi(argv[2]));
    tcp_work(srv_addr, 1);
    return 0;
}
