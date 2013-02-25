#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

void func1(void)
{
    printf("func1 says hi\n");
    //exit(0); -> nothing happens
}

void func2(void)
{
    printf("func2 says hi\n");
    //exit(0); -> nothing happens
}

int main(int argc, char const *argv[])
{
    atexit(func1);
    atexit(func2);

    long int a = sysconf(_SC_ATEXIT_MAX);
    printf("ATEXIT_MAX = %ld\n", a);

    //abort(); -> no atexit registered gets called
    printf("Main done!\n");
    return 0;
}
