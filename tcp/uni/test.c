#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
int makePDU(char *s, int pin)
{
    int i;
    for (i = 0; s[i] != '\n'; i++)
        ;
    s[i] = '\0';
    for (int j = 8; j < i + 8; j++)
        s[j] = s[j - 8];
    memset(s, 0, 8);
    s[3] = htonl(pin);
    s[7] = htonl(i + 1);
    return i + 1;
}
int main()
{
    char buf[120] = "abc\n";
    makePDU(buf, 12);
    printf("%d %d\n", ntohl((int)buf[3]), ntohl((int)buf[7]));
}