#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, const char* argv[], const char* envp[])
{
    const char* user = NULL;
    int i = 0;

    while (envp[i] != NULL)
    {
        if (strncmp("USER", envp[i], 4) == 0)
        {
            user = &envp[i][5];
            break;
        }

        ++i;
    }

    if (user == NULL)
    {
        fprintf(stderr, "Could not locale USER in environment variables.\n");
        return 1;
    }

    printf("Hello %s !\n", user);

    return 0;
}
