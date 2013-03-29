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
        pid_t pid2 = wait(NULL);
        if (pid2 != pid)
        {
            perror("wait");
            exit(EXIT_FAILURE);
        }

        printf("world !\n");
    }
    else // child
    {
        printf("hello ");
        fflush(stdout);
    }

    return 0;
}
