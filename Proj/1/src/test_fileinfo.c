#include <stdio.h>

#include "fileinfo.h"

int main()
{
    file_info fi;
    file_info_new(&fi, "ficheiro.txt");

    /*printf("debug: %s, %s, %d\n", __FILE__, __FUNCTION__, __LINE__);*/

    fi.state = STATE_ADDED;
    fi.iter = 10;

    char str[100];
    file_info_to_string(&fi, str);

    printf("%s\n", str);
    

    fi.state = STATE_INALTERED;
    file_info_to_string(&fi, str);
    printf("%s\n", str);
    fi.state = STATE_MODIFIED;
    file_info_to_string(&fi, str);
    printf("%s\n", str);
    fi.state = STATE_REMOVED;
    file_info_to_string(&fi, str);
    printf("%s\n", str);

    return 0;
}