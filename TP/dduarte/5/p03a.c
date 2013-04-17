#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include <sys/wait.h>

#define READ 0
#define WRITE 1

#define MAXLINE 256

int main(int argc, char const *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <file_name>\n", argv[0]);
        exit(1);
    }

    const char* filename = argv[1];

    FILE* fp = fopen(filename, "r");
    if (fp == NULL)
    {
        perror("fopen");
        exit(1);
    }

    int fd[2];
    if (pipe(fd) < 0)
    {
        perror("pipe");
        exit(1);
    }

    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork");
        exit(1);
    }
    else if (pid > 0) // parent
    {
        close(fd[READ]);

        char line[MAXLINE];
        while (fgets(line, MAXLINE, fp) != NULL)
        {
            int n = strlen(line);
            if (write(fd[WRITE], line, n) != n)
            {
                perror("write");
                exit(1);
            }
        }

        if (ferror(fp))
        {
            perror("ferror");
            exit(1);
        }

        close(fd[WRITE]);

        if (waitpid(pid, NULL, 0) < 0)
        {
            perror("waitpid");
            exit(1);
        }

        fclose(fp);
        exit(0);
    }
    else // child
    {
        close(fd[WRITE]);
        if (dup2(fd[READ], STDIN_FILENO) != STDIN_FILENO)
        {
            perror("dup2");
            exit(1);
        }

        if (execlp("sort", "sort", NULL) < 0)
        {
            perror("execlp");
            exit(1);
        }
    }

    return 0;
}
