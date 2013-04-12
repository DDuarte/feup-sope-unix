#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char* argv[], char* envp[])
{
    if (argc != 2) {
        printf("usage: %s dirname\n", argv[0]);
        exit(1);
    }

    pid_t pid = fork();
    if (pid > 0)
    {
        printf("My child is going to execute command \"ls -laR %s\"\n", argv[1]);

        int status;
        waitpid(pid, &status, 0);
        if (WIFSIGNALED(status) != 0)
        {
            printf("Command was killed with signal: %d\n", WTERMSIG(status));  
        }
        else
        {
            printf("Command exited with code: %d\n", WEXITSTATUS(status));
        }
    }
    else if (pid == 0)
    {
        // a)
        execlp("ls", "ls", "-laR", argv[1]);

        printf("Command not executed !\n");
        exit(1);
    }
    exit(0);
}