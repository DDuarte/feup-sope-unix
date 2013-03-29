#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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
        write(STDOUT_FILENO, "Hello ", strlen("Hello "));
    }
    else // child
    {
        while (getppid() != 1); // init pid

        pid_t pid2 = fork();
        if (pid2 < 0)
        {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (pid2 > 0)
        {
            write(STDOUT_FILENO, "my ", strlen("my "));
        }
        else
        {
            while (getppid() != 1); // init pid
            write(STDOUT_FILENO, "friends!\n", strlen("friends!\n"));
        }
    }

    return 0;
}
