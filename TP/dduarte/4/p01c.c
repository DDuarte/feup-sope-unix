// PROGRAMA p01c.c

#include <stdio.h>
#include <sys/types.h>
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

    struct sigaction act;

    act.sa_handler = sigint_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    if (sigaction(SIGINT, &act, NULL) != 0)
    {
        fprintf(stderr,"Unable to install SIGINT handler\n");
        exit(1);
    }

    int sleeptime = 30;

    do
    {
        printf("Sleeping for %u seconds ...\n", sleeptime);
        sleeptime = sleep(sleeptime);
    } while (sleeptime != 0);

    printf("Waking up ...\n");
    exit(0);
}
