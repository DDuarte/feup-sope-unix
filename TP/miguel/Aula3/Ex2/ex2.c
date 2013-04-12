#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char const *argv[])
{
    // write(STDOUT_FILENO, "1", 1);
    printf("1"); // printf("1\n"); // newline character forces stdout flush
    // fflush(stdout); // Empties printf buffer before fork
    if (fork() > 0)
    {
        // write(STDOUT_FILENO, "2", 1);
        // write(STDOUT_FILENO, "3", 1);
        printf("2");
        printf("3");
    }   
    else
    {
        // write(STDOUT_FILENO, "4", 1);
        // write(STDOUT_FILENO, "5", 1);
        printf("4");
        printf("5");
    } 

    // write(STDOUT_FILENO, "\n", 1);
    printf("\n");
    return 0;
}