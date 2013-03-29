// PROGRAMA p01b.c
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

void sigint_handler(int signo)
{
    printf("In SIGINT handler ...\n");
}

int main(void)
{
    printf("use \"kill -INT %u\"\n", getpid());

    if (signal(SIGINT, sigint_handler) < 0)
    {
        fprintf(stderr,"Unable to install SIGINT handler\n");
        exit(1);
    }

    nanosleep

    int sleeptime = 30;

    do
    {
        printf("Sleeping for %u seconds ...\n", sleeptime);
        sleeptime = sleep(sleeptime);
    } while (sleeptime != 0);

    printf("Waking up ...\n");
    exit(0);
}
