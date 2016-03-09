#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define BUFFER_SIZE 1

int main(int argc, char *argv[])
{
	// Control argv strlen
	printf("-----Control argv: %d\n", strlen(argv[2]));

	// Variables
	int lineNumber = 0,
		columnNumber = 0,
		countLetter = 0,
		totalWord = 0;

	char currentChar;
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

		int countArgv = 0;

		while (read(openFile, &currentChar, BUFFER_SIZE) > 0)
		{
			// Control word
			if (currentChar == argv[2][countArgv])
			{
				++countLetter;
				++countArgv;

				if (countLetter == strlen(argv[2]))
				{
					// write();

					++totalWord;

					countLetter = 0;
					// i = -1; // I think!
				}
			}
			else
			{
				countLetter = 0;
				countArgv = 0;
			}



			if (currentChar != '\0') ++columnNumber;
			if (currentChar == '\n') ++lineNumber;
			printf("%c", currentChar);

		}

		// Control totalWord
		printf("\n-----Control totalWord : %d\n", totalWord);
	}


	// Control line & column number
	printf("\nLine number: \t%d", lineNumber);
	printf("\nColumn number: \t%d", columnNumber);
	printf("\n");
	return 0;
}

// Control 
// printf("\n-----Control : %d\n", openFile);