#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char const *argv[])
{
    char prog[20];

    sprintf(prog, "%s.c", argv[1]);
    execlp("gcc", "gcc", prog, "-Wall", "-o", argv[1], NULL);
    printf("Error calling gcc.");
    exit(1);
}