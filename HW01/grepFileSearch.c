#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define BUFFER_SIZE 1

int main(int argc, char *argv[])
{
	// Variables
	int currentLineNumber = 1,	// currentLineNumber is line counter
	curentColumnNumber = 1,		// curentColumnNumber is column counter
	countLetter = 0,			// countLetter need to while loop
	totalWord = 0;				// totalWord find searching text in the file

	char currentChar;
	ssize_t bytesRead;


	// Usage:
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
	int openFileForReading = open(argv[1], O_RDONLY);

	if (openFileForReading == -1)
	{
		printf("Error for opening file.\nExiting..\n");
		return 1;
	}
	else
	{
		// Searching text length
		int searchingTextLength = strlen(argv[2]);

		int countArgv = 0;

		while (read(openFileForReading, &currentChar, BUFFER_SIZE) > 0)
		{
			if (currentChar != '\0') ++curentColumnNumber;
			if (currentChar == '\n')
			{
				++currentLineNumber;
				curentColumnNumber = 1;
			}

			// Control word
			if (currentChar == argv[2][countArgv])
			{
				++countLetter;
				++countArgv;
				
				if (countLetter == strlen(argv[2]))
				{
					++totalWord;

					// Write a file
					FILE *openFileForWriting = fopen("gfF.log", "a+");
					fprintf(openFileForWriting, "%s dosyasında => %s metni dosya içinde %d satir ve %d sutunda bulundu.\n", argv[1], argv[2], currentLineNumber, curentColumnNumber - strlen(argv[2]) );
					fclose(openFileForWriting);
		
					// Must be zero 
					countLetter = 0;
				}
			}
			else
			{
				countLetter = 0 ;
				countArgv = 0;
			}

		}

		// Close reading file
		close(openFileForReading);

		// Ayıraç
		FILE *openFileForWriting = fopen("gfF.log", "a+");
		fprintf(openFileForWriting, "----------------------------------------------------------------");
		fclose(openFileForWriting);
			
		// Control totalWord
		printf("Aradığınız kelime %d kez bulunmuştur.\n", totalWord);
	}
	return 0;
}