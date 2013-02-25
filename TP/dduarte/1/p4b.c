#include <stdio.h>
#include <stdlib.h>

int main(int argc, const char* argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "2 argument are required (name and repeat number).\n");
        return 1;
    }

    // To use a name with spaces this program needs to be called with './p4a "John Doe" 3'
    const char* name = argv[1];
    int count = atoi(argv[2]);

    for (int i = 0; i < count; ++i)
        printf("Hello %s!\n", name);

    return 0;
}
