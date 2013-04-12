#include <stdio.h>
#include <stdlib.h>

FILE* sourceFile = NULL;
FILE* destFile = NULL;
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

	sourceFile = fopen(sourceName, "r");
	if (!sourceFile)
	{
		printf("Error opening source file: %s\n", sourceName);
		return 1;
	}

	destFile = fopen(destName, "w");
	if (!destFile)
	{
		printf("Error opening destination file: %s\n", destName);
		return 1;
	}

	fseek(sourceFile, 0, SEEK_END);
	size_t sourceSize = ftell(sourceFile);
	rewind(sourceFile);

	sourceContents = (char*) malloc(sourceSize * sizeof(char));

	int bufferSize = fread(sourceContents, sizeof(char), sourceSize, sourceFile);
	if (bufferSize != sourceSize)
	{
		printf("Error reading source file.\n");
		return 1;
	}

	int destSize = fwrite(sourceContents, sizeof(char), bufferSize, destFile);
	if (destSize != bufferSize)
	{
		printf("Error writing to destination file.\n");
		return 1;
	}

	return 0;
}

void exitHandler(void)
{
	if (sourceFile)
		fclose(sourceFile);
	if (destFile)
		fclose(destFile);
	if (sourceContents)
		free(sourceContents);
}