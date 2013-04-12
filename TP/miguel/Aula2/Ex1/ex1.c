#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char const *argv[], char const *envp[])
{
	char pass[50] = "";
	struct termios old, new;
	tcgetattr(STDIN_FILENO, &old);
	new = old;
	new.c_lflag &= ~(ECHO | ECHOE | ECHOK | ECHONL | ICANON);

	tcsetattr(STDIN_FILENO, TCSAFLUSH, &new);

	printf ("Pass ? ");

	for (char c = getc(stdin); c != EOF && c != '\n'; c = getc(stdin))
	{
		strncat(pass, &c, 1);
		printf("*");
	}

  	printf("\n%s\n", pass);

  	tcsetattr(STDIN_FILENO, TCSANOW, &old);

	return 0;
}