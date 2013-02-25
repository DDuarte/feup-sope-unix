#include <stdlib.h>
#include <stdio.h>

void func1(void)
{
    printf("func1 says hi\n");
}

void func2(void)
{
    printf("func2 says hi\n");
}

int main(int argc, char const *argv[])
{
    atexit(func1);
    atexit(func2);
    printf("Main done!\n");
    return 0;
}
