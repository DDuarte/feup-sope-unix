#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

static bool up = true;

void sigusr_handler(int signo)
{
    if (signo == SIGUSR1)
        up = true;
    else if (signo == SIGUSR2)
        up = false;
}

int main(int argc, char const *argv[])
{
    struct sigaction action;

    action.sa_handler = sigusr_handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;

    if (sigaction(SIGUSR1, &action, NULL) < 0)
    {
        fprintf(stderr, "Unable to install SIGUSR1 handler\n");
        exit(1);
    }

    if (sigaction(SIGUSR2, &action, NULL) < 0)
    {
        fprintf(stderr, "Unable to install SIGUSR2 handler\n");
        exit(1);
    }

    int var = 0;

    while (true)
    {
        sleep(1);
        var = var + (up ? 1 : -1);
        printf("%d\n", var);
    }
    return 0;
}