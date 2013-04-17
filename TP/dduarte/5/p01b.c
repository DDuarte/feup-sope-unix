#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#define READ 0
#define WRITE 1

typedef struct nums_t
{
    int num1;
    int num2;
} nums;

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
        nums numbers;
        printf("First number: ");
        scanf("%d", &numbers.num1);
        printf("Second number: ");
        scanf("%d", &numbers.num2);

        close(fd[READ]);
        write(fd[WRITE], &numbers, sizeof(nums));
    }
    else // child
    {
        nums numbers;

        close(fd[WRITE]);
        read(fd[READ], &numbers, sizeof(nums));

        int sum = numbers.num1 + numbers.num2;
        int diff = numbers.num1 - numbers.num2;
        int mul = numbers.num1 * numbers.num2;
        float div;
        if (numbers.num2 != 0)
            div = (float)numbers.num1 / (float)numbers.num2;
        else
            div = NAN;

        printf("Sum: %d, Diff: %d, Mul: %d, Div: %f\n", sum, diff, mul, div);
    }
    return 0;
}
