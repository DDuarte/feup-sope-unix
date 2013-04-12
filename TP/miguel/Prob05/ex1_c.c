#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>
#include <string.h>

#define READ 0
#define WRITE 1

#define NUMS_TO_READ 2
#define NUMS_MAX_SIZE 10

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
        char stringValues[NUMS_TO_READ][NUMS_MAX_SIZE];
        int values[2];

        read(fd[READ], stringValues, sizeof(stringValues));

        values[0] = atoi(stringValues[0]);
        values[1] = atoi(stringValues[1]);

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
        char values[NUMS_TO_READ][NUMS_MAX_SIZE];

        printf("Insert the first integer: ");
        fflush(stdout);

        scanf("%10s", values[0]);

        printf("Insert the second integer: ");
        fflush(stdout);

        scanf("%10s", values[1]);

        write(fd[WRITE], values, sizeof(values));

        wait(NULL);

        exit(0);
    }
}