#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>

#define READ 0
#define WRITE 1

#define NUMS_TO_READ 2

typedef enum { TYPE_INTEGER, TYPE_FLOAT, TYPE_INVALID } type;

typedef struct { 
    type t; 
    union { 
        int i; 
        float f;
    } val;
} value;

const char strOperations[4][11] = {
    "%d + %d = ",
    "%d - %d = ",
    "%d * %d = ",
    "%d / %d = "
};

void sendValue(int fd, type t, void* val)
{
    value valToSend;

    valToSend.t = t;

    switch (t)
    {
    case TYPE_INTEGER:
        valToSend.val.i = *((int*)val);
        break;
    case TYPE_FLOAT:
        valToSend.val.f = *((float*)val);
        break;
    default:
        break;
    }

    write(fd, &valToSend, sizeof(value));
}

value receiveValue(int fd)
{
    value valToReceive;

    read(fd, &valToReceive, sizeof(value));

    return valToReceive;
}

int main(int argc, char const *argv[])
{
    int fd1[2];
    int fd2[2];

    if (pipe(fd1) == -1)
    {
        perror("pipe");
        exit(1);
    }

    if (pipe(fd2) == -1)
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
        close(fd1[WRITE]);
        close(fd2[READ]);

        int values[2];

        read(fd1[READ], &values, sizeof(values));

        close(fd1[READ]);

        int sum =  values[0] + values[1];
        int diff = values[0] - values[1];
        int prod = values[0] * values[1];

        float quocient;
        if (values[1] != 0)
            quocient = (float)values[0] / (float)values[1];
        else
            quocient = NAN;


        sendValue(fd2[WRITE], TYPE_INTEGER, &sum);
        sendValue(fd2[WRITE], TYPE_INTEGER, &diff);
        sendValue(fd2[WRITE], TYPE_INTEGER, &prod);
        if (isnan(quocient))
            sendValue(fd2[WRITE], TYPE_INVALID, NULL);
        else
            sendValue(fd2[WRITE], TYPE_FLOAT, &quocient);

        close(fd2[WRITE]);

        _exit(0);
    }
    else if (pid > 0) // Father process -> Writer
    {
        close(fd1[READ]);
        close(fd2[WRITE]);
        int values[NUMS_TO_READ];

        printf("Insert two comma separated integers (example: 1, 2): ");
        fflush(stdout);

        scanf("%10d, %10d", &values[0], &values[1]);

        write(fd1[WRITE], values, sizeof(values));

        close(fd1[WRITE]);

        for (int i = 0; i < 4; ++i)
        {
            value val = receiveValue(fd2[READ]);
            printf(strOperations[i], values[0], values[1]);

            switch(val.t)
            {
                case TYPE_INTEGER:
                    printf("%d", val.val.i);
                    break;
                case TYPE_FLOAT:
                    printf("%f", val.val.f);
                    break;
                case TYPE_INVALID:
                    printf("invalid");
                    break;
            }

            printf("\n");
        }

        close(fd2[READ]);
        wait(NULL);

        exit(0);
    }
}