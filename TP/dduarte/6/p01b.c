// PROGRAMA p01.c
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#define STDERR 2
#define NUMITER 10

void *thrfunc1(void *arg)
{
    //int i;
    fprintf(stderr, "Starting thread %d\n", *(int *) arg);
    //for (i = 1; i <= NUMITER; i++)
    //    write(STDERR, arg, 1);
    return NULL;
}

void *thrfunc2(void *arg)
{
    //int i;
    fprintf(stderr, "Starting thread %s\n", (char *) arg);
    //for (i = 1; i <= NUMITER; i++)
    //    write(STDERR, arg, 1);
    return NULL;
}

int main()
{
    int arg1 = 1;
    char* arg2 = "2";

    pthread_t ta, tb;
    pthread_create(&ta, NULL, thrfunc1, &arg1);
    pthread_create(&tb, NULL, thrfunc2, arg2);
    pthread_join(ta, NULL);
    pthread_join(tb, NULL);

    return 0;
}
