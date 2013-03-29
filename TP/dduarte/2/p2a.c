#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>


int main(int argc, char const *argv[])
{
    if (argc != 3)
    {
        printf("Usage: copy file1 file2\n");
        return EXIT_FAILURE;
    }

    const char* srcName = argv[1];
    const char* dstName = argv[2];

    FILE* src = fopen(srcName, "r");
    FILE* dst = fopen(dstName, "w");

    char buffer[512];

    int rb;
    while ((rb = fread(buffer, sizeof(char), 512, src)) != 0)
    {
        if (fwrite(buffer, sizeof(char), rb, dst) != rb)
        {
            fprintf(stderr, "%s\n", );
        }
    }

    return 0;
}
