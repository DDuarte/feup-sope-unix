#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

int main(int argc, char const *argv[])
{
    int returnCode = 0, sourcefd = -1, destfd = -1;

    if (argc < 2 || argc > 3)
    {
        printf("Usage: %s <source> <dest>[terminal]\n", argv[0]);
        returnCode = 1;
        goto ret;
    }

    sourcefd = open(argv[1], O_RDONLY);
    if(sourcefd == -1)
    {
        perror("Error opening source file");
        returnCode = 1;
        goto ret;
    }

    destfd = -1;
    if (argc == 3)
    {
        struct stat buf;
        int res = fstat(sourcefd, &buf);
        if (res != 0)
        {
            perror("Error reading source file permissions");
            returnCode = 1;
            goto ret;
        }

        destfd = open(argv[2], O_CREAT | O_EXCL | O_WRONLY, buf.st_mode);
        if (destfd == -1)
        {
            perror("Error opening destination file");
            returnCode = 1;
            goto ret;
        }
        dup2(destfd, STDOUT_FILENO);
    }

    char buffer[50];

    for (int size = read(sourcefd, buffer, 50); size != 0; size = read(sourcefd, buffer, 50))
    {
        write(STDOUT_FILENO, buffer, size);
    }

ret:
    close(sourcefd);
    close(destfd);

    return returnCode;
}