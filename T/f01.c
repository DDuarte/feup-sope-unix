#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc, char const *argv[])
{
    printf("before fork...\n");

    fork();

    printf("I'm process %d. My parent is process %d.\n", getpid(), getppid());

    return 0;
}
