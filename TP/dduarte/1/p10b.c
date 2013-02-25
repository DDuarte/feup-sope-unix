#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_BUFFER 101
#define MAX_TOKENS 10

int main(int argc, char const *argv[])
{
    char line[MAX_BUFFER];
    char* tokens[MAX_TOKENS];

    if (fgets(line, MAX_BUFFER - 1, stdin) == NULL)
    {
        printf("Error while reading line from stdin.\n");
        return EXIT_FAILURE;
    }

    int count = 0;
    char* tok = strtok(line, " ");
    while (count < MAX_TOKENS && tok != NULL)
    {
        tokens[count] = tok;
        tok = strtok(NULL, " ");
        count++;
    }

    for (int i = 0; i < count; ++i)
        printf("%s\n", tokens[i]);

    return EXIT_SUCCESS;
}
