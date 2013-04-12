#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>

#define READ 0
#define WRITE 1

#define NUMS_TO_READ 2

typedef struct values_t
{
    int num1;
    int num2;
} values;

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
        values integers;

        read(fd[READ], &integers, sizeof(values));

        printf("%d + %d = %d\n", integers.num1, integers.num2, integers.num1 + integers.num2);
        printf("%d - %d = %d\n", integers.num1, integers.num2, integers.num1 - integers.num2);
        printf("%d * %d = %d\n", integers.num1, integers.num2, integers.num1 * integers.num2);

        printf("%d / %d = ", integers.num1, integers.num2);
        if (integers.num2 != 0)
            printf("%f\n", (float)integers.num1 / (float)integers.num2);
        else
            printf("%f\n", NAN);

        close(fd[READ]);
        _exit(0);
    }
    else if (pid > 0) // Father process -> Writer
    {
        close(fd[READ]);
        values integers;

        printf("Insert two comma separated integers (example: 1, 2): ");
        fflush(stdout);

        scanf("%10d, %10d", &integers.num1, &integers.num2);

        write(fd[WRITE], &integers, sizeof(values));

        wait(NULL);

        exit(0);
    }
}