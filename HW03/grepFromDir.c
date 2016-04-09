#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

// Buffers
#define BUFFER_STREAM		1		// Stream
#define BUFFER_MINI			1024	// Mini
#define BUFFER_STANDART		2048	// Standart
#define BUFFER_LARGE		4096	// Large
#define BUFFER_ULTRA		8192	// Ultra

// Sizes
#define SIZE_256 256
#define FILE_NAME_SIZE 256

// Modes
#define READ_ONLY (O_RDONLY) 
#define WRITE_ONLY (O_WRONLY)
#define FIFO_PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#define FILE_CREAT_WRITE_APPEND (O_CREAT | O_WRONLY | O_APPEND)

void fileCheck(char *, char *);

int searchInFile(const char *, const char *, const char *, const int);

void writePipe(const int, const int);

void writeFifo(const char *, int);

void readFifo(void);

void writeLogFile(const char *);

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

	// Fifo variable
	char fifoFileName[FILE_NAME_SIZE];

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
			
			// Fifo
			snprintf(fifoFileName, sizeof(fifoFileName), "%d", (int)getppid());
			if (mkfifo(fifoFileName, FIFO_PERMS) == -1)
				perror("mkfifo");

			// Create pipe and child 
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

				// Fifo

				// Pipe for reading
				int status;
				char results[BUFFER_STANDART];
				close(pipeFileDescription[1]);

				if (0 < (status = read(pipeFileDescription[0], results, sizeof(results))))
				{
					printf("%s%d\n", results, status);
					writeLogFile(results); // Write pipe file result
				}

				// Control
				printf("--Status %d\n", status);



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
	char tempFileText[BUFFER_STANDART];
	char tempFileName[SIZE_256];

	// Create file path
	char newPath[SIZE_256];
	strcpy(newPath, filePath);
	strcat(newPath, "/");
	strcat(newPath, fileName);


		// Openin temp file for result
		snprintf(tempFileName, sizeof(tempFileName), "%d.txt", getpid());
		int tempFileDescriptor = open(tempFileName, FILE_CREAT_WRITE_APPEND);

		// Write header for result temp file
		snprintf(
			tempFileText,
			sizeof(tempFileText),
			"Path: %s\nFile: %s -> %s\n",
			filePath, fileName, searchingWord
		);
		write(tempFileDescriptor, tempFileText, strlen(tempFileText));

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
					write(tempFileDescriptor, tempFileText, strlen(tempFileText));

					// Must be zero
					countLetter = 0;
				}
			}
			else
			{
				countLetter = 0;
				countArgv = 0;
			}

		} // end while

		// End of file
		snprintf(tempFileText, sizeof(tempFileText), "------------------------------------\n");
		write(tempFileDescriptor, tempFileText, strlen(tempFileText));

		// Close tempfile
		close(tempFileDescriptor);

		// Close reading file
		close(openFileForReadingHandle);

		// If find to ant word, all results write pipe or unlink file
		if (0 < totalWord)
		{
			// All results writing pipe file and close temp
			tempFileDescriptor = open(tempFileName, O_RDONLY);

			// Write from temp file to pipe
			writePipe(tempFileDescriptor, pipeFileDescription);

			// Close temp file
			close(tempFileDescriptor);
		}

	} // end if else

	// Delete temp file
	unlink(tempFileName);

	return 0;

} // end function searchInFile

//
//	FUNCTION:	writePipe
//
//	SUPPOSE:	All results are writing from temp file to pipe file
//
//	COMMENTS(TR)
//
void writePipe(const int tempFileDescriptor, const int pipeFileHandle)
{
	// Variable
	char tempText[BUFFER_ULTRA];

	read(tempFileDescriptor, tempText, BUFFER_ULTRA);
	write(pipeFileHandle, tempText, BUFFER_ULTRA);

	return;
}

void writeLogFile(const char *results)
{
	int logFileHandle = open("gfD.log", FILE_CREAT_WRITE_APPEND);
	write(logFileHandle, results, strlen(results));
	close(logFileHandle);

	return;
}

//
//	FUNCTION:	newPath
//
//	SUPPOSE:	Create new path for file and folder
//
//	COMMENTS(TR)
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

void writeFifo(const char *resultsFromPipe, int fifoFileName)
{
	char fifoFilename[FILE_NAME_SIZE];
	snprintf(fifoFilename, sizeof(fifoFilename), "%d", getppid());

	fifoDescripton = open(fifoName, WRITE_ONLY);

	write(fifoDescripton, resultsFromPipe, strlen(resultsFromPipe));

	close(fifoDescripton);
}

void readFifo(void)
{
	char fifoFilename[FILE_NAME_SIZE];
	snprintf(fifoFilename, sizeof(fifoFilename), "%d", getpid());

	char fifoBuffer[BUFFER_STANDART];

	fifoDescripton = open(fifoName, READ_ONLY);

	read(fifoDescripton, fifoBuffer, strlen(fifoBuffer));
	printf("%s\n", fifoBuffer);

	close(fifoDescripton);
}