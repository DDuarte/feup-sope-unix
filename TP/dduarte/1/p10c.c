#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_BUFFER 101
#define MAX_TOKENS 10

int main(int argc, char const *argv[])
{
    char line[MAX_BUFFER];
    //char* tokens[MAX_TOKENS];

    if (fgets(line, MAX_BUFFER - 1, stdin) == NULL)
    {
        printf("Error while reading line from stdin.\n");
        return EXIT_FAILURE;
    }

    char *str1, *str2, *token, *subtoken;
    char *saveptr1, *saveptr2;
    int j;

    for (j = 1, str1 = line; ; j++, str1 = NULL)
    {
        token = __strtok_r(str1, "|;", &saveptr1);
        if (token == NULL)
            break;
        printf("%d: %s\n", j, token);

        for (str2 = token; ; str2 = NULL)
        {
            subtoken = __strtok_r(str2, " ", &saveptr2);
            if (subtoken == NULL)
                break;
            printf(" --> %s\n", subtoken);
        }
    }

    return EXIT_SUCCESS;
}
