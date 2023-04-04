#include <sys/types.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define IPLENGTH 9
#define PORTLENGTH 5
#define IPH 0 
#define IPN 1
#define PORTH 2
#define PORTN 3

u_int32_t s_to_ipv4(char *s);
u_int16_t s_to_port(char *s);
void toUpperCaseHex(char *s, u_int32_t p, int len);
void addPrefixZero(char* s, int len);
void reversePrint(char *prefix, char *s, int len);
void printHex(char* hex, u_int32_t hexnumber, int LENGTH, int flag);

int main(int argc, char **argv)
{
    u_int32_t ipv4_n = inet_addr(argv[1]), ipv4_h = ntohl(ipv4_n);
    u_int16_t port_h, port_n;

    struct in_addr addr_h, addr_n;
    inet_aton(argv[1], &addr_n);
    memcpy(&addr_h, &ipv4_h, sizeof(ipv4_h));
    port_h = s_to_port(argv[2]);

    port_n = htons(port_h);
    u_int16_t temp = ntohs(port_n);

    char hex_ipv4_h[IPLENGTH];
    char hex_ipv4_n[IPLENGTH];
    char hex_port_h[PORTLENGTH];
    char hex_port_n[PORTLENGTH];

    printf("[ipv4_n_cbc_d]%s\n", inet_ntoa(addr_n));

    printHex(hex_ipv4_n, ipv4_n, IPLENGTH, IPN);
    printHex(hex_port_n, port_n, PORTLENGTH, PORTN);

    printf("[port_n_asw_d]%d\n",port_n);
    printf("[ipv4_h_cbc_d]%s\n", inet_ntoa(addr_h));

    printHex(hex_ipv4_h, ipv4_h, IPLENGTH, IPH);
    printHex(hex_port_h, port_h, PORTLENGTH, PORTH);

    printf("[port_h_asw_d]%d\n",port_h);
    return 0;
}

u_int16_t s_to_port(char *s)
{
    return (u_int16_t)atoi(s);
}
void toUpperCaseHex(char *s, u_int32_t p, int len)
{
    sprintf(s, "%x", p);
    for (int i = 0; i < len; ++i)
        if (s[i] <= 'z' && s[i] >= 'A')
            s[i] ^= 32;
    addPrefixZero(s, len);
}
void addPrefixZero(char* s, int len)
{
    int i;
    for (i = 0; i < len; ++i)
        if (s[i] == '\0')
            break;
    if (i == len - 1)
        return;
    int temp = i;
    for (i; i >= 0; --i)
        s[i + len - temp - 1] = s[i];
    for (int i = 0; i < len - temp - 1; ++i)
        s[i] = '0';
}
void reversePrint(char *prefix, char *s, int len)
{
    printf("%s", prefix);
    for (int i = len - 3; i >= 0; i -= 2)
    {
        printf("%c%c", s[i], s[i + 1]);
    }
    printf("\n");
}
void printHex(char* hex, u_int32_t hexnumber, int LENGTH, int flag)
{
    toUpperCaseHex(hex, hexnumber, LENGTH);
    if(flag == IPH)
    {
        reversePrint("[ipv4_h_cbc_x]",hex, LENGTH);
        printf("[ipv4_h_asw_x]%s\n", hex);
    }
    else if (flag == IPN)
    {
        reversePrint("[ipv4_n_cbc_x]",hex, LENGTH);
        printf("[ipv4_n_asw_x]%s\n", hex);
    }
    else if (flag == PORTH)
    {
        reversePrint("[port_h_cbc_x]",hex, LENGTH);
        printf("[port_h_asw_x]%s\n", hex);
    }
    else
    {
        reversePrint("[port_n_cbc_x]",hex, LENGTH);
        printf("[port_n_asw_x]%s\n", hex);
    }
}