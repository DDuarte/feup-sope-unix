#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[], char *envp[])
{
    pid_t pid;
    if (argc < 2)
    {
        printf("usage: %s <dirname>\n", argv[0]);
        exit(1);
    }

    char* output = NULL;
    if (argc == 3)
        output = argv[2];

    pid = fork();
    if (pid > 0)
        printf("My child is going to execute command \"ls -laR %s\"\n", argv[1]);
    else if (pid == 0)
    {
        if (output)
        {
            int fd = creat(output, 0777);
            if (fd < 0)
                perror("creat");
            else
                dup2(fd, STDOUT_FILENO);
        }

        execlp("ls", "ls", "-laR", argv[1]);
        printf("Command not executed !\n");
        exit(1);
    }

    exit(0);
}
