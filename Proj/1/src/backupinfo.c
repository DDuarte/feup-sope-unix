#include <stdio.h>
#include <assert.h>

#include "backupinfo.h"
#include "vector.h"
#include "fileinfo.h"

void backup_info_new(backup_info* bi)
{
    assert(bi);

    bi->iter = -1;
    vector_new(&bi->file_list);
}

int backup_info_read(DIR* dst, int iter, backup_info* result)
{
    fprintf(stderr, "Function '%s' isn't implemented!\n", __FUNCTION__);
    return -1;
}

int backup_info_write(DIR* dst, const backup_info* backup)
{
    fprintf(stderr, "Function '%s' isn't implemented!\n", __FUNCTION__);
    return -1;
}