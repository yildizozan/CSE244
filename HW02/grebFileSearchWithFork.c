#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define BUFFER_SIZE 1
#define PATH_SIZE 255

void fileCheck (char *currentPath, char *searchText);
int searching(char *filePath, char *searchingText);

int main(int argc, char *argv[])
{
	// Using:
	if (argc != 3)
	{
		printf("Using: ./grD [directory] text\n");
		return 1;
	}

	fileCheck(argv[1], argv[2]);

	return 0;
}

void fileCheck (char *currentPath, char *searchText)
{
	// Folder variables
	DIR *dir;
	struct dirent *ent;

	// Try to open folder
	if ((dir = opendir(currentPath)) == NULL)
	{
		perror("opendir");
		exit(EXIT_FAILURE);
	}
	else // Successfull open dir
	{
		// Reading directioary
		while ((ent = readdir(dir)) != NULL)
		{
			pid_t childPid = fork();

			// Child process can't be created
			if (childPid < 0)
			{
				perror("no child\n");
				exit(EXIT_FAILURE);
			}
			else if(childPid == 0) // Process is success
			{
				// NO CWD and upper
				if(strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
					exit(EXIT_FAILURE);

				// Control for folder
				if (ent->d_type == DT_DIR)
				{
					// Create new path
					char newPath[255];
					strcpy(newPath, currentPath);
					strcat(newPath, "/");
					strcat(newPath, ent->d_name);

					// Filecheck new path
					fileCheck(newPath, searchText);
					return;
				}
				// Control for file
				else if(ent->d_type == DT_REG)
				{
					// File path
					char tempPath[255];
					strcpy(tempPath, currentPath);
					strcat(tempPath, "/");
					strcat(tempPath, ent->d_name);

					// Searching new path
					searching(tempPath, searchText);

					//printf("%s dosyasında ->\n", currentPath);
					printf("%s -> \tpid: %d -> parent: %d\n", tempPath, getpid(), getppid());
					return;
				}
			}
			else  // Parent process after fork succeeds
			{
			    wait(NULL);
			} // end else

		}
		closedir(dir);

	} // end else

}

int searching(char *filePath, char *searchingText)
{
	// Variables
	int currentLineNumber = 1,	// currentLineNumber is line counter
		curentColumnNumber = 1,		// curentColumnNumber is column counter
		countLetter = 0,			// countLetter need to while loop
		totalWord = 0;				// totalWord find searching text in the file

	char currentChar;

	// Opening file (READ ONLY MODE)
	int openFileForReading = open(filePath, O_RDONLY);

	if (openFileForReading == -1)
	{
		printf("Error for opening file.\nExiting..\n");
		return 1;
	}
	else
	{
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
			if (currentChar == searchingText[countArgv])
			{
				++countLetter;
				++countArgv;

				if (countLetter == strlen(searchingText))
				{
					++totalWord;

					// Write a file
					FILE *openFileForWriting = fopen("gfF.log", "a+");
					fprintf(openFileForWriting, "%s file => %s word, %d line and %d column found.\n", filePath, searchingText, currentLineNumber, curentColumnNumber - strlen(searchingText));
					fclose(openFileForWriting);

					// Must be zero
					countLetter = 0;
				}
			}
			else
			{
				countLetter = 0;
				countArgv = 0;
			}

		}

		// Close reading file
		close(openFileForReading);
/*
		// Ayıraç
		FILE *openFileForWriting = fopen("gfF.log", "a+");
		fprintf(openFileForWriting, "----------------------------------------------------------------\n");
		fclose(openFileForWriting);
*/
		// Control totalWord
		printf("Aradığınız kelime %d kez bulunmuştur.\n", totalWord);
	}
	return 0;
}

// NOTS
// ent variable name change.