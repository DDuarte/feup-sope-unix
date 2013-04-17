#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include <sys/wait.h>

#define READ 0
#define WRITE 1

int main(int argc, char const *argv[])
{
    if (argc != 3)
    {
        printf("Usage: %s <dir> <arg>\n", argv[0]);
        exit(1);
    }

    const char* dir = argv[1];
    const char* arg = argv[2];

    int fd1[2];
    pipe(fd1);

    pid_t pid1 = fork();
    if (pid1 > 0) // parent
    {
        close(fd1[READ]);
        dup2(fd1[WRITE], STDOUT_FILENO);
        execlp("ls", "ls", dir, "-laR", NULL);
    }
    else // child
    {
        int fd2[2];
        pipe(fd2);

        close(fd1[WRITE]);

        pid_t pid2 = fork();
        if (pid2 > 0) // parent
        {
            close(fd2[READ]);
            dup2(fd1[READ], STDIN_FILENO);
            dup2(fd2[WRITE], STDOUT_FILENO);
            execlp("grep", "grep", arg, NULL);
        }
        else // child
        {
            close(fd2[WRITE]);
            dup2(fd2[READ], STDIN_FILENO);
            execlp("sort", "sort", NULL);
        }
    }

    return 0;
}
