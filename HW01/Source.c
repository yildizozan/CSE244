#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define BUFFER_SIZE 1

int main(int argc, char *argv[])
{
	// Variables
	int lineNumber = 0;
	int columnNumber = 0;
						// Read Text
	char byteCurrent;
	ssize_t bytesRead;


	// Usage
	if (argc == 1)
	{
		printf("Usage: ./file [directory or file] [searching text]\n");
		return 1;
	}
	else if (argc != 3)
	{
		printf("Please enter 2 parameters!\n");
		return 1;
	}

	// Opening file (READ ONLY MODE)
	int openFile = open(argv[1], O_RDONLY);

	if (openFile == -1)
	{
		printf("Error for opening file.\nExiting..\n");
		return 1;
	}
	else
	{
		// Searching text length
		int searchingTextLength = strlen(argv[2]);

		// Control ++++++++++++
		//bytesRead = read(openFile, &byteCurrent, BUFFER_SIZE);
		//printf("BytesRead: %zd\n\n", bytesRead);

		while (read(openFile, &byteCurrent, BUFFER_SIZE) > 0)
		{
			if (byteCurrent != '\0') columnNumber++;
			if (byteCurrent == '\n') lineNumber++;
			printf("%c", byteCurrent);
			

		}
	}


	// Control line & column number
	printf("\nLine number: \t%d", lineNumber);
	printf("\nColumn number: \t%d", columnNumber);
	printf("\n");
	return 0;
}

// Control 
// printf("\n-----Control : %d\n", openFile);
