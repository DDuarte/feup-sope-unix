#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "fileinfo.h"

void file_info_new(file_info* fi, const char* fileName)
{
    assert(fi);

    if (!fileName)
    {
        fi->fileName = NULL;
    }
    else
    {
        int size = strlen(fileName);
        fi->fileName = (char*) malloc(size * sizeof(char));
        strcpy(fi->fileName, fileName);
    }

    fi->state = STATE_INALTERED;
    fi->iter = -1;
}

void file_info_free(file_info* fi)
{
    assert(fi);

    if (fi->fileName)
        free(fi->fileName);
}

void file_info_set_name(file_info* fi, const char* fileName)
{
    assert(fi);
    assert(fileName);

    int newSize = strlen(fileName);

    if (fi->fileName) 
    {
        int prevSize = strlen(fi->fileName);

        if (newSize > prevSize)
        {
            fi->fileName = (char*) realloc(fi->fileName, newSize);
        }
    }
    else
    {
        fi->fileName = (char*) malloc(newSize);
    }

    strcpy(fi->fileName, fileName);
}

void file_info_to_string(file_info* fi, char* dest)
{
    assert(fi);
    assert(dest);

    sprintf(dest, "%c %d %s", (char)fi->state, fi->iter, fi->fileName);
}