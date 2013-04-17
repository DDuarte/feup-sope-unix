#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#define READ 0
#define WRITE 1
#define NUM_MAX_LEN 10

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
        char num1[NUM_MAX_LEN], num2[NUM_MAX_LEN];
        printf("First number: ");
        scanf("%10s", num1);
        printf("Second number: ");
        scanf("%10s", num2);

        close(fd[READ]);
        write(fd[WRITE], &num1, NUM_MAX_LEN);
        write(fd[WRITE], &num2, NUM_MAX_LEN);
    }
    else // child
    {
        char num1s[NUM_MAX_LEN], num2s[NUM_MAX_LEN];
        close(fd[WRITE]);
        read(fd[READ], &num1s, NUM_MAX_LEN);
        read(fd[READ], &num2s, NUM_MAX_LEN);

        int num1 = atoi(num1s);
        int num2 = atoi(num2s);

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
