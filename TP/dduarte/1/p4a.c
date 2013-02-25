#include <stdio.h>

int main(int argc, const char* argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "1 argument is required.\n");
        return 1;
    }

    // To use a name with spaces this program needs to be called with './p4a "John Doe"'
    const char* name = argv[1];
    printf("Hello %s!\n", name);
    return 0;
}
