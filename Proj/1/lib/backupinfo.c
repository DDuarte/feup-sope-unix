#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "backupinfo.h"
#include "vector.h"
#include "fileinfo.h"

void backup_info_new(backup_info* bi)
{
    assert(bi);

    bi->iter = -1;
    vector_new(&bi->file_list);
}

int backup_info_read(FILE* source, backup_info* result)
{
    assert(source);
    assert(result);

    int iter;

    if (fscanf(source, "%d\n", &iter) == EOF)
        return EOF;

    result->iter = iter;

    file_info tempFi;
    file_info_new(&tempFi, NULL);

    while (file_info_read(source, &tempFi) != EOF)
    {
        file_info* fi = NULL;
        file_info_copy(&tempFi, &fi);
        vector_push_back(&result->file_list, fi);
    }


    file_info_free(&tempFi);

    return 0;
}

int backup_info_write(FILE* dest, const backup_info* backup)
{
    assert(dest);
    assert(backup);

    char fileInfoBuffer[1000] = "";

    fprintf(dest, "%d\n", backup->iter);

    for (int i = 0; i < vector_size(&backup->file_list); ++i)
    {
        file_info_to_string((file_info*)vector_get(&backup->file_list, i), fileInfoBuffer);
        fprintf(dest, "%s\n", fileInfoBuffer);
    }

    return 0;
}

void backup_info_add_file(backup_info* bi, file_info* fi)
{
    assert(bi);
    assert(fi);

    file_info* fiToBeAdded = NULL;
    file_info_copy(fi, &fiToBeAdded);
    vector_push_back(&bi->file_list, fiToBeAdded);
}
