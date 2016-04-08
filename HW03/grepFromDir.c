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
#define SIZE_256 256

void fileCheck(char *, char *);

int searchInFile(const char *, const char *, const char *, const int);

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
			// NO CWD and upper
			if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
				continue;
			
			// Child process can't be created
			if (pipe(pipeFileDescription) < 0 || (childPid = fork()) < 0)
			{
				perror("pipe\n");
				perror("fork\n");
				exit(EXIT_FAILURE);
			}
			else if (childPid) // Parent process
			{
				// We'll wait all chil process
				int childStatus; 
				waitpid(childPid, &childStatus, 0);

				// Pipe for reading
				int status;
				char childName[BUFFER_MINI];
				close(pipeFileDescription[1]);
				if (0 < (status = read(pipeFileDescription[0], childName, sizeof(childName))))
				//status = read(pipeFileDescription[0], childName, sizeof(childName));
					printf("%s%d\n", childName, status);

				close(pipeFileDescription[0]);
			}
			else  // Child process
			{
				// Control for folder
				if (ent->d_type == DT_DIR) // Is this folder?
				{
					// Create new path
					char newPath[SIZE_256];
					strcpy(newPath, currentPath);
					strcat(newPath, "/");
					strcat(newPath, ent->d_name);

					// Filecheck new path
					fileCheck(newPath, searchText);

					// Successful
					exit(EXIT_SUCCESS);
				}
				else if (ent->d_type == DT_REG) // Is this file
				{
					// Child name for pipe
					char childName[BUFFER_MINI];
					snprintf(childName, sizeof(childName), "%s - Childpid:%d", ent->d_name, (int)getpid());
					
					// Pipe
					close(pipeFileDescription[0]);

					// Searching new path
					searchInFile(currentPath, ent->d_name, searchText, pipeFileDescription[1]);

					close(pipeFileDescription[1]);

					// Successful
					exit(EXIT_SUCCESS);
				}

			} // end else

		} // end while

		closedir(dir);

	} // end else

}

int searchInFile(const char *filePath, const char *fileName, const char *searchingWord, const int pipeFileDescription)
{
	// Variables
	int currentLineNumber = 1,		// currentLineNumber is line counter
		curentColumnNumber = 1,		// curentColumnNumber is column counter
		countLetter = 0,			// countLetter need to while loop
		totalWord = 0;				// totalWord find searching text in the file

	char currentChar;

	// Temp file variables
	char tempFileText[SIZE_256];

	// Create file path
	char newPath[SIZE_256];
	strcpy(newPath, filePath);
	strcat(newPath, "/");
	strcat(newPath, fileName);


		// Openin temp file for result
		snprintf(tempFileText, sizeof(tempFileText), "%d.txt", getpid());
		int tempFileHandle = open(tempFileText, O_CREAT | O_WRONLY | O_APPEND);

		// Write header for result temp file
		snprintf(
			tempFileText,
			sizeof(tempFileText),
			"%s\n%s -> %s\n",
			filePath, fileName, searchingWord
		);
		write(tempFileHandle, tempFileText, strlen(tempFileText));

	// Opening file for searching (READ ONLY MODE)
	int openFileForReadingHandle = open(newPath, O_RDONLY);

	if (openFileForReadingHandle == -1)
	{
		printf("Error for opening file.\nExiting..\n");
		return 1;
	}
	else
	{
		int countArgv = 0;

		while (read(openFileForReadingHandle, &currentChar, BUFFER_STREAM) > 0)
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
					
					// Results are writing to temp file.
					snprintf(tempFileText,
						sizeof(tempFileText),
						"\t%d line, %d column found.\n",
						currentLineNumber,
						curentColumnNumber - strlen(searchingWord)
					);
					write(tempFileHandle, tempFileText, strlen(tempFileText));

					// Writing pipe
					write(pipeFileDescription, tempFileText, strlen(tempFileText));

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

		// End of file
		snprintf(tempFileText, sizeof(tempFileText), "-----------------------------------\n");
			write(tempFileHandle, tempFileText, strlen(tempFileText));

		// Close reading file
		close(openFileForReadingHandle);

		// If find to ant word, all results write pipe or unlink file
		if (totalWord == 0)
		{
			unlink(newPath);
		}
		else
		{
			//
		}
	}

	return 0;
}

//
// FUNCTION:	newPath
//
// SUPPOSE:		Create new path for file and folder
//
const char *createNewPath(const char *oldPath, const char *name)
{
	// Create file path
	char *newPath;
	strcpy(newPath, oldPath);
	strcat(newPath, "/");
	strcat(newPath, name);

	return newPath;
}