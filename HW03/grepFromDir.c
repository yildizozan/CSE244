#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

// Buffers
#define BUFFER_STREAM 1
#define BUFFER_MINI 1024

// Path size
#define PATH_SIZE 255

void fileCheck(char *, char *);

int searching(char *, char *, char *, int);

int main(int argc, char *argv[])
{
	/*
	// Using:
	if (argc != 3)
	{
		printf("Using: ./grD [directory] text\n");
		return 1;
	}
	*/

	fileCheck(argv[1], argv[2]);

	return 0;
}

void fileCheck(char *currentPath, char *searchText)
{
	// Folder variables
	DIR *dir;
	struct dirent *ent;

	// Fork variable
	pid_t childPid;

	// Pipe variable
	int pipeFileDescription[2];

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
			
			// Child process can't be created
			if (pipe(pipeFileDescription) < 0|| (childPid = fork()) < 0)
			{
				perror("no child\n");
				exit(EXIT_FAILURE);
			}
			else if (childPid) // Parent process
			{
				// We'll wait all chil process
				wait(NULL);

				// Pipe for reading
				char childName[100];
				close(pipeFileDescription[1]);
				read(pipeFileDescription[0], childName, sizeof(childName));
				printf("%s\n", childName);
				close(pipeFileDescription[0]);

			}
			else  // Child process
			{

				// Control
				// Child name for pipe
				char childName[100];
				snprintf(childName, sizeof(childName), "Child name: %d", (int)getpid());
				
				//Pipe
				close(pipeFileDescription[0]);
				write(pipeFileDescription[1], childName, sizeof(childName));
				close(pipeFileDescription[1]);

				// NO CWD and upper
				if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
					exit(EXIT_FAILURE);

				// Control for folder
				if (ent->d_type == DT_DIR)
				{
					// Create new path
					char newPath[PATH_SIZE];
					strcpy(newPath, currentPath);
					strcat(newPath, "/");
					strcat(newPath, ent->d_name);

					// Filecheck new path
					fileCheck(newPath, searchText);
					exit(EXIT_SUCCESS);
				}
				// Control for file
				else if (ent->d_type == DT_REG)
				{

					// Searching new path
					searching(currentPath, ent->d_name, searchText, pipeFileDescription[1]);

					exit(EXIT_SUCCESS);
				}

			} // end else

		}
		closedir(dir);

	} // end else

}

int searching(char *filePath, char *fileName, char *searchingWord, int pipeFileDescription)
{
	// Create file path
	char newPath[PATH_SIZE];
	strcpy(newPath, filePath);
	strcat(newPath, "/");
	strcat(newPath, fileName);

	// Variables
	int currentLineNumber = 1,		// currentLineNumber is line counter
		curentColumnNumber = 1,		// curentColumnNumber is column counter
		countLetter = 0,			// countLetter need to while loop
		totalWord = 0;				// totalWord find searching text in the file

	char currentChar;

	// Openin temp file for result


	// Opening file for searching (READ ONLY MODE)
	int openFileForReading = open(newPath, O_RDONLY);

	if (openFileForReading == -1)
	{
		printf("Error for opening file.\nExiting..\n");
		return 1;
	}
	else
	{
		int countArgv = 0;

		while (read(openFileForReading, &currentChar, BUFFER_STREAM) > 0)
		{
			if (currentChar != '\0') 
				++curentColumnNumber;

			if (currentChar == '\n')
			{
				++currentLineNumber;
				curentColumnNumber = 1;
			}

			// Control word
			if (currentChar == searchingWord[countArgv])
			{
				++countLetter;
				++countArgv;

				if (countLetter == strlen(searchingWord))
				{
					++totalWord;

					// Write a file
					FILE *openFileForWriting = fopen("gfD.log", "a+");
					fprintf(openFileForWriting,
						"%s -> %d line and %zu column found.\n",
						fileName,
						currentLineNumber,
						curentColumnNumber - strlen(searchingWord));
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
	}
	return 0;
}