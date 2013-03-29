// p1.c

#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

int main(void)
{
    char password[50];

    struct termios oldTer, newTer;
    tcgetattr(STDIN_FILENO, &oldTer);

    newTer = oldTer;

    newTer.c_lflag &= ~(ECHO | ECHOE | ECHOK | ECHONL | ICANON);

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &newTer);

    char c;
    int i = 0;
    do
    {
        c = fgetc(stdin);
        password[i++] = c;
        if (c != '\n')
            fputc('*', stdout);
        else
            fputc('\n', stdout);
    }
    while (c != '\n' && i < 50);

    password[i - 1] = '\0';

    printf("Password: %s\n", password);

    tcsetattr(STDIN_FILENO, TCSANOW, &oldTer);

    return 0;
}
