#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void addSuffixZero(char* data, int bitLength)
{
    int i = 0;
    for (; data[i]!='\0'; i++);
    memset((data + i), '0', bitLength - i);
}
int checksum(const char *data, int length)
{
    int sum = 0;
    for (int i = 0; i < length ; i += 4)
    {
        int value;
        char temp[4] = {0};
        strncpy(temp, data + i, 4);
        addSuffixZero(temp, 4);
        sscanf(temp, "%04x", &value);
        sum += value;
        if (sum > 0xffff)
            sum += 1;
        sum &= 0xffff;
    }
    return 0xffff - sum;
}
int checksum2(const char *data, int length)
{
    int sum = 0;
    for (int i = 0; i < length ; i += 2)
    {
        int value;
        char temp[2] = {0};
        strncpy(temp, data + i, 2);
        addSuffixZero(temp, 2);
        sscanf(temp, "%02x", &value);
        sum += value;
        if (sum > 0xff)
            sum += 1;
        sum &= 0xff;
    }
    return 0xff - sum;
}
int main(int argc, char **argv)
{

    const char *s = argv[1];
    printf("%04x\n", checksum(s, strlen(s)));
    printf("%02x\n", checksum2(s, strlen(s)));
    return 0;
}