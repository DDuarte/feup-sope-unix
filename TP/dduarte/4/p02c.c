// PROGRAMA p02c.c

#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

void sigint_handler(int signo)
{
    printf("Entering SIGINT handler ...\n");
    sleep(10);
    printf("Exiting SIGINT handler ...\n");
}

void sigterm_handler(int signo)
{
    printf("Entering SIGTERM handler ...\n");
    printf("Exiting SIGTERM handler ...\n");
}

int main(void)
{
    printf("My pid: %u\n", getpid());

    struct sigaction action1;
    action1.sa_handler = sigint_handler;
    sigemptyset(&action1.sa_mask);
    action1.sa_flags = 0;

    if (sigaction(SIGINT, &action1, NULL) < 0)
    {
        fprintf(stderr,"Unable to install SIGINT handler\n");
        exit(1);
    }

    struct sigaction action2;
    action2.sa_handler = sigterm_handler;
    sigemptyset(&action2.sa_mask);
    action2.sa_flags = 0;

    if (sigaction(SIGTERM, &action2, NULL) < 0)
    {
        fprintf(stderr,"Unable to install SIGTERM handler\n");
        exit(1);
    }

    printf("Try me with CTRL-C ...\n");
    while(1)
        pause();

    exit(0);
}
