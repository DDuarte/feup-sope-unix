#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main(void)
{
    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid > 0) // parent
    {
        printf("hello ");
    }
    else // child
    {
        while (getppid() != 1); // init pid
        printf("world !\n");
    }

    return 0;
}
