#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int sourceFile = -1;
int destFile = -1;
char* sourceContents = NULL;

void exitHandler(void);

int main(int argc, char const *argv[], char const *envp[])
{

	if (argc != 3)
	{
		printf("Usage: %s <source> <dest>\n", argv[0]);
		return 1;
	}

	atexit(exitHandler);

	const char* sourceName = argv[1];
	const char* destName = argv[2];

	sourceFile = open(sourceName, O_RDONLY);
	if (sourceFile == -1)
	{
		perror("Error opening source file");
		return 1;
	}

	destFile = open(destName, O_CREAT | O_EXCL | O_WRONLY);
	if (destFile == -1)
	{
		perror("Error opening destination file");
		return 1;
	}

	size_t sourceSize = lseek(sourceFile, 0, SEEK_END);
	lseek(sourceFile, 0, 0);

	sourceContents = (char*) malloc(sourceSize * sizeof(char));

	int bufferSize = read(sourceFile, sourceContents, sourceSize);
	if (bufferSize != sourceSize)
	{
		printf("Error reading source file.\n");
		return 1;
	}

	int destSize = write(destFile, sourceContents, bufferSize);
	if (destSize != bufferSize)
	{
		perror("Error writing to destination file");
		return 1;
	}

	return 0;
}

void exitHandler(void)
{
	if (sourceFile)
		close(sourceFile);
	if (destFile)
		close(destFile);
	if (sourceContents)
		free(sourceContents);
}