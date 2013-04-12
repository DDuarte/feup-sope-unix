#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char const *argv[])
{
    int file = open("file.txt", O_APPEND);
    char name[50] = "";
    int  grade = 0;
    bool success = true;

    while (success)
    {
        int error;
        printf("name? ");
        fgets(name, 50, stdin);
        if (name == NULL)
        {
            success = false;
            break;
        }
        printf("grade? ");
        error = scanf("%d", &grade);
        if (error == EOF)
        {
            success = false;
            break;
        }
        char value[70];
        sprintf(value, "%s, %d", name, grade);
        write(file, value, strlen(value));
    }

    close(file);

    return 0;
}