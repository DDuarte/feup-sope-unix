#include <signal.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>

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
    pid_t pid = fork();

    if (pid == -1)
    {
        fprintf(stderr, "Unable to create child process.\n");
        exit(1);
    }
    else if (pid > 0)
    {
        srand(time(NULL));

        for (int i = 0; i < 50; i++)
        {
            sleep(1);
            if (rand() % 2)
            {
                kill(pid, SIGUSR1);
            }
            else
            {
                kill(pid, SIGUSR2);
            }

        }
        wait(NULL);
    }
    else
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

        for (int i = 0; i < 50; i++)
        {
            sleep(1);
            var = var + (up ? 1 : -1);
            printf("%d\n", var);
        }
    }
    
    exit(0);
}