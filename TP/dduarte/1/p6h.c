#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define BUF_LENGTH 256

int main(int argc, const char* argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage: p6h file1 file2\n");
        exit(1);
    }

    FILE *src, *dst;
    char buf[BUF_LENGTH];
    if ( ( src = fopen( argv[1], "r" ) ) == NULL )
    {
        //fprintf(stderr, "Error: %i\n", errno);
        perror("Error");
        exit(1);
    }
    if ( ( dst = fopen( argv[2], "w" ) ) == NULL )
    {
        //fprintf(stderr, "Error: %i\n", errno);
        perror("Error");
        exit(2);
    }

    while( ( fgets( buf, BUF_LENGTH, src ) ) != NULL )
    {
        fputs( buf, dst );
    }
    fclose( src );
    fclose( dst );
    exit(0); // zero é geralmente indicativo de "sucesso"
}
