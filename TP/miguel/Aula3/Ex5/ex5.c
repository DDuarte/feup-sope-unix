#include <sys/types.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char const *argv[])
{
    
    pid_t pid = fork();

    if (pid > 0)
    {
        write(STDOUT_FILENO, "Hello ", strlen("Hello "));
    }
    else
    {
        while (getppid() != 1) { }
        pid_t pid1 = fork();
        if (pid1 > 0)
        {
            write(STDOUT_FILENO, "my ", strlen("my "));
        }
        else
        {
            while (getppid() != 1) { }
            write(STDOUT_FILENO, "friends!\n", strlen("friends!\n"));
        }
    }

    return 0;
}