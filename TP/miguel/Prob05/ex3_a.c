#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define READ 0
#define WRITE 1

void printUsage(char const *argv[])
{
    assert(argv != NULL);
    printf("Usage: %s <fileName>", argv[0]);
}

int readFile(const char* fileName, char** contents)
{
    int fd = open(fileName, O_RDONLY);

    size_t size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, 0);

    *contents = (char*) malloc(size * sizeof(char));

    int bufferSize = read(fd, *contents, size);
    if (bufferSize != size)
    {
        return 0;
    }

    return bufferSize;
}

int main(int argc, char const *argv[])
{
    if (argc != 2)
    {
        printUsage(argv);
        exit(1);
    }

    int pipefd[2];

    if (pipe(pipefd) == -1)
    {
        perror("pipe");
        exit(1);
    }



    pid_t pid = fork();

    if (pid < 0)
    {
        perror("fork");
        exit(1);
    }
    else if (pid == 0)
    {
        close(pipefd[WRITE]);
        dup2(pipefd[READ], STDIN_FILENO);
        execlp("sort", "sort", NULL);
    }
    else if (pid > 0)
    {
        close(pipefd[READ]);

        char* contents;
        int size  = readFile(argv[1], &contents);
        if (size == 0)
        {
            free(contents);
            fprintf(stderr, "Error reading file.\n");
            exit(1);
        }

        write(pipefd[WRITE], contents, size * sizeof(char));

        close(pipefd[WRITE]);
        free(contents);

        wait(NULL);
    }

    exit(0);
}