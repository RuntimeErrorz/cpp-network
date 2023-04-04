#include <stdio.h>
int main()
{
    struct X1
    {
        int b;
        short* c;
        char a[3];
    };
    struct X2
    {
        char a[3];
        struct X1 b[3];
        short d[3];
    };
    printf("%ld\n", sizeof(struct X1));
    printf("%ld\n", sizeof(struct X2));
}