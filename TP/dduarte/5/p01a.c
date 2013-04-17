#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#define READ 0
#define WRITE 1

int main(int argc, char const *argv[])
{
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
        int num1, num2;
        printf("First number: ");
        scanf("%d", &num1);
        printf("Second number: ");
        scanf("%d", &num2);

        close(fd[READ]);
        write(fd[WRITE], &num1, sizeof(int));
        write(fd[WRITE], &num2, sizeof(int));
    }
    else // child
    {
        int num1, num2;
        close(fd[WRITE]);
        read(fd[READ], &num1, sizeof(int));
        read(fd[READ], &num2, sizeof(int));

        int sum = num1 + num2;
        int diff = num1 - num2;
        int mul = num1 * num2;
        float div;
        if (num2 != 0)
            div = (float)num1 / (float)num2;
        else
            div = NAN;

        printf("Sum: %d, Diff: %d, Mul: %d, Div: %f\n", sum, diff, mul, div);
    }

    return 0;
}
