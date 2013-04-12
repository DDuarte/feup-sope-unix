// PROGRAMA p01a.c
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
    unsigned int timeLeft = 30;
    struct sigaction new_action;
    


    new_action.sa_handler = sigint_handler;
    sigemptyset(&new_action.sa_mask);
    new_action.sa_flags = 0;

    
    if (sigaction(SIGINT, &new_action, NULL) < 0)
    {
        fprintf(stderr, "Unable to install SIGINT handler\n");
        exit(1);
    }

    printf("Sleeping for 30 seconds ...\n");

    
    while(timeLeft > 0) { timeLeft = sleep(timeLeft); }
    
    printf("Waking up ...\n");

    exit(0);
}