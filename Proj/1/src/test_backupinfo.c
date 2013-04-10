#include <stdio.h>

#include "backupinfo.h"
#include "vector.h"

int main()
{
    backup_info bi;

    backup_info_new(&bi);
    
    printf("Iteration : %d\n", bi.iter);
    printf("Vector Size : %d\n", vector_size(&bi.file_list));
    printf("Vector Capacity : %d\n", vector_capacity(&bi.file_list));



    return 0;
}