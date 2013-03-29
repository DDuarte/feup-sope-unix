#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

int main(void)
{
    pid_t pid;
    int i, j;
    printf("I'm process %d. My parent is %d.\n", getpid(),getppid());
    for (i = 1; i <= 3; i++)
    {
        pid = fork();
        if ( pid < 0)
        {
            printf("fork error");
            exit(1);
        }
        else if (pid == 0)
        {
            printf("I'm process %d. My parent is %d. I'm going to work for 1 second ...\n", getpid(), getppid());
            sleep(1); // simulando o trabalho do filho
            printf("I'm process %d. My parent is %d. I finished my work\n", getpid(), getppid());
            exit(0); // a eliminar na alinea c)
        }
        else
        {
            // simulando o trabalho do pai
            for (j = 1; j <= 10; j++)
            {
                waitpid(-1, NULL, WNOHANG);
                sleep(1);
                printf("father working ...\n");
            }
        }
    }

    exit(0);
}


// Alternative:
/* POSIX.1-2001  specifies  that  if  the disposition of SIGCHLD is set to
       SIG_IGN or the SA_NOCLDWAIT flag is set for SIGCHLD (see sigaction(2)),
       then children that terminate do not become zombies and a call to wait()
       or waitpid() will block until all children have  terminated,  and  then
       fail  with  errno set to ECHILD.
*/
