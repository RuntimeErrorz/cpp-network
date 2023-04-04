#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <bits/types/sigset_t.h>
int main()
{
    for (int i = 3; ~i; --i)
    {
        printf("SynX\n");
    }
}