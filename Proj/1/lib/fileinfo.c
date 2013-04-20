#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "fileinfo.h"

void file_info_new(file_info* fi, const char* file_name)
{
    assert(fi);
    fi->file_name = NULL;

    if (file_name)
        file_info_set_name(fi, file_name);

    fi->state = STATE_INALTERED;
    fi->iter = -1;
}

void file_info_free(file_info* fi)
{
    assert(fi);

    if (fi->file_name)
    {
        free(fi->file_name);
        fi->file_name = NULL;
    }
}

void file_info_set_name(file_info* fi, const char* file_name)
{
    assert(fi);
    assert(file_name);

    int newSize = strlen(file_name) + 1;

    if (fi->file_name)
    {
        int prevSize = strlen(fi->file_name);

        if (newSize > prevSize)
            fi->file_name = (char*) realloc(fi->file_name, newSize * sizeof(char));
    }
    else
        fi->file_name = (char*) malloc(newSize * sizeof(char));

    strcpy(fi->file_name, file_name);
}

void file_info_to_string(file_info* fi, char* dest)
{
    assert(fi);
    assert(dest);

    sprintf(dest, "%c %d %s", (char)fi->state, fi->iter, fi->file_name);
}

int file_info_read(FILE* source, file_info* result)
{
    assert(source);
    assert(result);

    char st;
    int iter;
    char name_buffer[1000];

    if (fscanf(source, "%c %d ", &st, &iter) == EOF)
        return EOF;

    fgets(name_buffer, 1000, source);

    if (feof(source))
        return EOF;

    int size = strlen(name_buffer);
    if (name_buffer[size-1] == '\n')
        name_buffer[size-1] = '\0';

    result->state = st;
    result->iter = iter;
    file_info_set_name(result, name_buffer);

    return 0;
}

void file_info_copy(const file_info* source, file_info** dest)
{
    if (!*dest)
        *dest = (file_info*) malloc(sizeof(file_info));

    file_info_new(*dest, source ? source->file_name : NULL);

    if (source)
    {
        (*dest)->iter = source->iter;
        (*dest)->state = source->state;
    }
}
