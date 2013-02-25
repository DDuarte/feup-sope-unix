#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char const *argv[])
{
    char line[101];

    if (fgets(line, 100, stdin) == NULL)
    {
        printf("Error while reading line from stdin.\n");
        return EXIT_FAILURE;
    }

    char* tok = strtok(line, " ");
    while (tok != NULL)
    {
        printf("%s\n", tok);
        tok = strtok(NULL, " ");
    }

    return EXIT_SUCCESS;
}
