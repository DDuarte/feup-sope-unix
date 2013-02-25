#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, const char* argv[], const char* envp[])
{
    char* user = getenv("USER");

    if (user == NULL)
    {
        fprintf(stderr, "Could not locale USER in environment variables.\n");
        return 1;
    }

    printf("Hello %s !\n", user);

    return 0;
}
