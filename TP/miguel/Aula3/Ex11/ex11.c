#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define COMMAND_SIZE 200

#define WRITE_PROMPT() write(STDOUT_FILENO, prompt, promptSize)
#define READ_COMMAND() read(STDIN_FILENO, command, COMMAND_SIZE)
static const char* prompt = "minish >";
static const int   promptSize = strlen("minish >");
static char command[COMMAND_SIZE];

int main(int argc, char const *argv[])
{
    WRITE_PROMPT();
    int error = READ_COMMAND();

    while(strncmp(command, "quit", 4) != 0 && error != -1)
    {
        pid_t pid = fork();
        if (pid > 0)
        {
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
        else
        {
            if (argc == 3) 
            {
                int fd = open(argv[2], O_CREAT | O_WRONLY | O_EXCL);
                if (fd != -1)
                    dup2(fd, STDOUT_FILENO);
                else
                    printf("Could not create file %s", argv[2]);
            }

            // a)
            execlp("ls", "ls", "-laR", argv[1]);

            printf("Command not executed !\n");
        }
    }

    return 0;
}