#include <stdio.h>
#include <stdlib.h>

#include "backupinfo.h"
#include "fileinfo.h"
#include "vector.h"

int FileSelector(const struct dirent* file)
{
    return file->d_type == DT_REG;
}


int main()
{
    backup_info bi;

    backup_info_new(&bi);
    
    FILE* fi = fopen("dir.txt", "r");

    backup_info_read(fi, &bi);

    /*bi.iter = 0;*/

    /*struct dirent** files;*/

    /*int res = scandir("/home/miguel", &files, FileSelector, alphasort);*/

/*
    for (int i = 0; i < res; ++i)
    {
        file_info* fi = (file_info*) malloc(sizeof(file_info));
        file_info_new(fi, files[i]->d_name);
        fi->iter = 0;
        fi->state = STATE_ADDED;
        vector_push_back(&bi.file_list, fi);
    }
    */
   
   for (int i = 0; i < vector_size(&bi.file_list); ++i)
   {
        file_info* file_i = vector_get(&bi.file_list, i);
        printf("%c %s %d\n", file_i->state, file_i->fileName, file_i->iter);
   }

    printf("Iteration : %d\n", bi.iter);
    printf("Vector Size : %d\n", vector_size(&bi.file_list));
    printf("Vector Capacity : %d\n", vector_capacity(&bi.file_list));

    fclose(fi);

    return 0;
}