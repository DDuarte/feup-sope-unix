#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>

#define READ 0
#define WRITE 1

#define NUMS_TO_READ 2

int main(int argc, char const *argv[])
{
    int fd[2];

    if (pipe(fd) == -1)
    {
        perror("pipe");
        exit(1);
    }

    pid_t pid;

    pid = fork();

    if (pid < 0)
    {
        perror("fork");
        exit(1);
    }
    else if (pid == 0) // Child process -> Reader
    {
        close(fd[WRITE]);
        int values[2];

        read(fd[READ], &values, sizeof(values));

        printf("%d + %d = %d\n", values[0], values[1], values[0] + values[1]);
        printf("%d - %d = %d\n", values[0], values[1], values[0] - values[1]);
        printf("%d * %d = %d\n", values[0], values[1], values[0] * values[1]);

        printf("%d / %d = ", values[0], values[1]);
        if (values[1] != 0)
            printf("%f\n", (float)values[0] / (float)values[1]);
        else
            printf("%f\n", NAN);

        close(fd[READ]);
        _exit(0);
    }
    else if (pid > 0) // Father process -> Writer
    {
        close(fd[READ]);
        int values[NUMS_TO_READ];

        printf("Insert two comma separated integers (example: 1, 2): ");
        fflush(stdout);

        scanf("%10d, %10d", &values[0], &values[1]);

        write(fd[WRITE], values, sizeof(values));

        wait(NULL);

        exit(0);
    }
}