#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "fileinfo.h"

void file_info_new(file_info* fi, const char* fileName)
{
    assert(fi);
    fi->fileName = NULL;

    if (fileName)
        file_info_set_name(fi, fileName);

    fi->state = STATE_INALTERED;
    fi->iter = -1;
}

void file_info_free(file_info* fi)
{
    assert(fi);

    if (fi->fileName)
    {
        free(fi->fileName);
        fi->fileName = NULL;
    }
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
            fi->fileName = (char*) realloc(fi->fileName, newSize);
    }
    else
        fi->fileName = (char*) malloc(newSize);

    strcpy(fi->fileName, fileName);
}

void file_info_to_string(file_info* fi, char* dest)
{
    assert(fi);
    assert(dest);

    sprintf(dest, "%c %d %s", (char)fi->state, fi->iter, fi->fileName);
}

int file_info_read(FILE* source, file_info* result)
{
    assert(source);
    assert(result);

    char st;
    int iter;
    char nameBuffer[1000];

    if (fscanf(source, "%c %d ", &st, &iter) == EOF)
        return EOF;

    fgets(nameBuffer, 1000, source);

    if (feof(source))
        return EOF;

    int size = strlen(nameBuffer);
    if (nameBuffer[size-1] == '\n')
        nameBuffer[size-1] = '\0';

    result->state = st;
    result->iter = iter;
    file_info_set_name(result, nameBuffer);

    return 0;
}

void file_info_copy(const file_info* source, file_info** dest)
{
    if (!*dest)
        *dest = (file_info*) malloc(sizeof(file_info));

    file_info_new(*dest, source ? source->fileName : NULL);

    if (source)
    {
        (*dest)->iter = source->iter;
        (*dest)->state = source->state;
    }
}
