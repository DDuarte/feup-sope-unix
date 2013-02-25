#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, const char* argv[], const char* envp[])
{
    // export USER_NAME="John Doe"
    char* user = getenv("USER_NAME");

    if (user == NULL)
    {
        fprintf(stderr, "Could not locale USER_NAME in environment variables.\n");
        return 1;
    }

    printf("Hello %s !\n", user);

    return 0;
}
