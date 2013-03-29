#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

// Description: this program calls gcc with exec and compiles the file passed as the 1st argument

int main(int argc, char *argv[])
{
    char prog[20];
    sprintf(prog, "%s.c", argv[1]);
    execlp("gcc", "gcc", prog, "-Wall", "-o", argv[1], NULL);
    printf("execlp(gcc) failed!\n");
    exit(1);
    //return 0;
}
