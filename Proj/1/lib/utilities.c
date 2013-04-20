#include "utilities.h"

#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <stdlib.h>

#define BUFFER_SIZE 1024

void iter_to_folder(int iter, const char* dst, time_t start_time, int dt, char** name)
{
    time_t ti = start_time + iter * dt;
    struct tm* timestruct = localtime(&ti);
    char buff[80];
    strftime(buff, 80, BACKUP_FOLDER_NAME_FORMAT, timestruct);

    int size = strlen(buff) + strlen(dst) + 1;
    *name = malloc((size + 1) * sizeof(char));

    sprintf(*name, "%s/%s", dst, buff);
    (*name)[size] = '\0';
}

void fork_copy_file(const char* src_dir, const char* dst_dir, const char* file_name)
{
    pid_t pid = fork();

    if (pid == 0)
    {
        copy_file(src_dir, dst_dir, file_name);
        exit(0);
    }
}

bool copy_file(const char* src_dir, const char* dst_dir, const char* file_name)
{
    int destfd = -1;
    bool return_code = true;

    char src_file_name[1024];
    sprintf(src_file_name, "%s/%s", src_dir, file_name);

    char dst_file_name[1024];
    sprintf(dst_file_name, "%s/%s", dst_dir, file_name);

    int sourcefd = open(src_file_name, O_RDONLY);
    if (sourcefd < 0)
    {
        perror("Error opening source file");
        return_code = false;
        goto ret;
    }

    struct stat buf;
    int res = fstat(sourcefd, &buf);
    if (res != 0)
    {
        perror("Error reading source file permissions");
        return_code = false;
        goto ret;
    }

    destfd = open(dst_file_name, O_CREAT | O_EXCL | O_WRONLY, buf.st_mode);
    if (destfd == -1)
    {
        perror("Error opening destination file");
        return_code = false;
        goto ret;
    }

    char buffer[BUFFER_SIZE];
    for (int size = read(sourcefd, buffer, BUFFER_SIZE); size != 0; size = read(sourcefd, buffer, BUFFER_SIZE))
        write(destfd, buffer, size);

ret:
    if (sourcefd != -1) close(sourcefd);
    if (destfd != -1) close(destfd);

    return return_code;
}
