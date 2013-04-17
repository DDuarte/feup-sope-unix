#include "utilities.h"

#include <string.h>
#include <stdio.h>
#include <malloc.h>

void iter_to_folder(int iter, const char* dst, time_t startTime, int dt, char** name)
{
    time_t ti = startTime + iter * dt;
    struct tm* timestruct = gmtime(&ti);
    char buff[80];
    strftime(buff, 80, "%Y_%m_%d_%H_%M_%S", timestruct);

    int size = strlen(buff) + strlen(dst) + 1;
    *name = malloc((size + 1) * sizeof(char));

    sprintf(*name, "%s/%s", dst, buff);
    (*name)[size] = '\0';
}
