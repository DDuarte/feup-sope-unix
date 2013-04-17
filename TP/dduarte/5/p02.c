#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#define READ 0
#define WRITE 1

typedef enum numbertype_t
{
    Int,
    Float,
    Invalid
} numbertype;

typedef struct number_t
{
    numbertype type;
    union
    {
        float f;
        int i;
    };
} number;

const char* operations_str[] = { "sum", "diff", "mul", "div" };

int main(int argc, char const *argv[])
{
    int fdpc[2];
    if (pipe(fdpc) < 0)
    {
        perror("pipe(fdpc)");
        exit(1);
    }

    int fdcp[2];
    if (pipe(fdcp) < 0)
    {
        perror("pipe(fdcp)");
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

        close(fdpc[READ]);
        close(fdcp[WRITE]);
        write(fdpc[WRITE], &num1, sizeof(int));
        write(fdpc[WRITE], &num2, sizeof(int));

        // ...

        number revcnum;

        close(fdcp[WRITE]);
        close(fdpc[READ]);
        for (int i = 0; i < 4; ++i)
        {
            read(fdcp[READ], &revcnum, sizeof(number));

            if (revcnum.type == Int)
                printf("%s %d\n", operations_str[i], revcnum.i);
            else if (revcnum.type == Float)
                printf("%s %f\n", operations_str[i], revcnum.f);
            else if (revcnum.type == Invalid)
                printf("%s invalid\n", operations_str[i]);
        }
    }
    else // child
    {
        int num1, num2;
        close(fdpc[WRITE]);
        close(fdcp[READ]);

        read(fdpc[READ], &num1, sizeof(int));
        read(fdpc[READ], &num2, sizeof(int));

        int sum = num1 + num2;
        int diff = num1 - num2;
        int mul = num1 * num2;
        float div;
        if (num2 != 0)
            div = (float)num1 / (float)num2;
        else
            div = NAN;

        number sumn;
        sumn.type = Int;
        sumn.i = sum;

        number diffn;
        diffn.type = Int;
        diffn.i = diff;

        number muln;
        muln.type = Int;
        muln.i = mul;

        number divn;
        divn.type = isnan(div) ? Invalid : Int;
        divn.f = div;

        close(fdcp[READ]);
        close(fdpc[WRITE]);
        write(fdcp[WRITE], &sumn, sizeof(number));
        write(fdcp[WRITE], &diffn, sizeof(number));
        write(fdcp[WRITE], &muln, sizeof(number));
        write(fdcp[WRITE], &divn, sizeof(number));
    }

    return 0;
}
