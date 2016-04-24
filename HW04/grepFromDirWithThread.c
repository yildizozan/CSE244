/***
 *                                                                                                                           
 *      ,ad8888ba,                                           8b        d8  88  88           88888888ba,    88  888888888888  
 *     d8"'    `"8b                                           Y8,    ,8P   88  88           88      `"8b   88           ,88  
 *    d8'        `8b                                           Y8,  ,8P    88  88           88        `8b  88         ,88"   
 *    88          88  888888888  ,adPPYYba,  8b,dPPYba,         "8aa8"     88  88           88         88  88       ,88"     
 *    88          88       a8P"  ""     `Y8  88P'   `"8a         `88'      88  88           88         88  88     ,88"       
 *    Y8,        ,8P    ,d8P'    ,adPPPPP88  88       88          88       88  88           88         8P  88   ,88"         
 *     Y8a.    .a8P   ,d8"       88,    ,88  88       88          88       88  88           88      .a8P   88  88"           
 *      `"Y8888Y"'    888888888  `"8bbdP"Y8  88       88          88       88  88888888888  88888888Y"'    88  888888888888  
 *                                                                                                                           
 *                                                                                                                           
 *                                                                                                                           
 *        88  8888888888    88     ,a8888a,             ,d8            ,d8       ,a8888a,      ad88888ba    ad888888b,       
 *      ,d88  88          ,d88   ,8P"'  `"Y8,         ,d888          ,d888     ,8P"'  `"Y8,   d8"     "8b  d8"     "88       
 *    888888  88  ____  888888  ,8P        Y8,      ,d8" 88        ,d8" 88    ,8P        Y8,  Y8a     a8P          a8P       
 *        88  88a8PPPP8b,   88  88          88    ,d8"   88      ,d8"   88    88          88   "Y8aaa8P"        ,d8P"        
 *        88  PP"     `8b   88  88          88  ,d8"     88    ,d8"     88    88          88   ,d8"""8b,      a8P"           
 *        88           d8   88  `8b        d8'  8888888888888  8888888888888  `8b        d8'  d8"     "8b   a8P'             
 *        88  Y8a     a8P   88   `8ba,  ,ad8'            88             88     `8ba,  ,ad8'   Y8a     a8P  d8"               
 *        88   "Y88888P"    88     "Y8888P"              88             88       "Y8888P"      "Y88888P"   88888888888       
 *                                                                                                                           
 *                                                                                                                           
 */
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

/* BUFFERS */
#define BUFFER_STREAM	1
#define BUFFER_SIZE		2048

/* SIZES */
#define FILE_NAME_SIZE 256

void fileCheck(char *, char *);

int searchInFile(const char *, const char *, const char *, const int);

void writePipe(const int, const int);

void writeLogFile(const char *);


int main(int argc, char *argv[])
{
	/* Buffer */
	char buffer[BUFFER_SIZE];
	char fifoCurrentName[FILE_NAME_SIZE];
	int fifoCurrentDescription;

	/* Usage */
	if (argc != 3)
	{
		printf("Using: ./grD [directory] text\n");
		return 1;
	}


	fileCheck(argv[1], argv[2]);

	while(0 < wait(NULL));

	/* If there are any data in process's fifo */
	if (0 < (fifoCurrentDescription = open(fifoCurrentName, O_RDONLY)))
		if (0 < read(fifoCurrentDescription, buffer, BUFFER_SIZE))
			writeLogFile(buffer);

	close(fifoCurrentDescription);
	unlink(fifoCurrentName);

	return 0;
} /* end main function */

/***************************************************

	FUNCTION:	fileCheck

	SUPPOSE:

	COMMENT:

		Olcak olacak olacak o kadar..

****************************************************/
void fileCheck(char *currentPath, char *searchText)
{
	/* Buffer */
	char buffer[BUFFER_SIZE];

	/* Folder variables */
	DIR *dir;
	struct dirent *ent;

	/* Fork variable */
	pid_t childPid;

	/* Pipe variable */
	int pipeDescription[2];

	/* Fork variable */
	char fifoUpperName[FILE_NAME_SIZE];
	char fifoCurrentName[FILE_NAME_SIZE];

	/* Fifo variables */
	int fifoCurrentDescription;
	int fifoUpperDescription;

	/* Try to open folder */
	if ((dir = opendir(currentPath)) == NULL)
	{
		perror("Error 122");
		exit(EXIT_FAILURE);
	}
	else /* Successfull open dir */
	{
		/* Reading directioary */
		while ((ent = readdir(dir)) != NULL)
		{
			/* NO CWD and upper */
			if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
				continue;

			/* Create pipe and child */
			sprintf(fifoCurrentName, "%d", (int)getpid());
			if (mkfifo(fifoCurrentName, 0666) < 0 && (errno != EEXIST))
			{
				perror("Error 222");
				exit(EXIT_FAILURE);
			}
			else
			{
				if (pipe(pipeDescription) < 0 || (childPid = fork()) < 0)
				{
					perror("Error 378");
					exit(EXIT_FAILURE);
				}
				else
				{
					/*
					 * 	Parent process
					 */
					if (childPid)
					{
						sprintf(fifoCurrentName, "%d", (int)getpid());
						sprintf(fifoUpperName, "%d", (int)getppid());

						close(pipeDescription[1]);

						/* Control for is process main or not? */
						if (0 < (fifoUpperDescription = open(fifoUpperName, O_WRONLY | O_NONBLOCK)))
						{
							printf("child pipe\n");
							/* If there are any data in pipe */
							if (0 < read(pipeDescription[0], buffer, BUFFER_SIZE))
								write(fifoUpperDescription, buffer, BUFFER_SIZE);

							printf("child fifo\n");
							if (0 < (fifoCurrentDescription = open(fifoCurrentName, O_RDONLY)))
							{
								printf("cuurent fifo\n");
								if (0 < read(fifoCurrentDescription, buffer, BUFFER_SIZE))
									write(fifoUpperDescription, buffer, BUFFER_SIZE);
							}

						}
						else
						{
							/* If there are any data in pipe */
							if (0 < read(pipeDescription[0], buffer, BUFFER_SIZE))
								writeLogFile(buffer);

							unlink(fifoCurrentName);


						} /* end if else (open fifo) */

						/* Close */
						close(pipeDescription[0]);

						close(fifoCurrentDescription);
						close(fifoUpperDescription);
					}
					else  /* Child process */
					{
						/* Control for folder */
						if (ent->d_type == DT_DIR) /* Is this folder? */
						{
							/* Create new path */
							char newPath[FILE_NAME_SIZE];
							strcpy(newPath, currentPath);
							strcat(newPath, "/");
							strcat(newPath, ent->d_name);

							/* Filecheck new path */
							fileCheck(newPath, searchText);

							exit(EXIT_SUCCESS);
						}
						else if (ent->d_type == DT_REG) /* Is this file */
						{
							/* Child name for pipe */
							char childName[BUFFER_SIZE];
							snprintf(childName, sizeof(childName), "%s - Childpid:%d", ent->d_name, (int)getpid());

							/* Pipe */
							close(pipeDescription[0]);

							/* Searching new path */
							searchInFile(currentPath, ent->d_name, searchText, pipeDescription[1]);

							close(pipeDescription[1]);

							exit(EXIT_SUCCESS);

						} /* end if else (dirent type) */

					} /* end else (chil pid control)*/

				} /* end if else (pipe) */

			} /* end if else (fifo) */

		} /* end while */

		closedir(dir);

	} /* end if else (opendir) */

} /* enf function (chechFile) */

/***************************************************

	FUNCTION:	searchInFile

	SUPPOSE:	Searching text in file

	COMMENT:

		Olcak olacak olacak o kadar..

****************************************************/
int searchInFile(const char *filePath, const char *fileName, const char *searchingWord, const int pipeDescription)
{
	/* Variables */
	int currentLineNumber = 1,		/* currentLineNumber is line counter */
		curentColumnNumber = 1,		/* curentColumnNumber is column counter */
		countLetter = 0,			/* countLetter need to while loop */
		totalWord = 0;				/* totalWord find searching text in the file */

	char currentChar;

	/* Temp file variables */
	char tempFileText[BUFFER_SIZE];
	char tempFileName[FILE_NAME_SIZE];

	/* Create file path */
	char newPath[FILE_NAME_SIZE];
	strcpy(newPath, filePath);
	strcat(newPath, "/");
	strcat(newPath, fileName);


		/* Openin temp file for result */
		snprintf(tempFileName, sizeof(tempFileName), "%d.txt", getpid());
		int tempFileDescriptor = open(tempFileName, O_CREAT | O_WRONLY | O_APPEND);

		/* Write header for result temp file */
		snprintf(
			tempFileText,
			sizeof(tempFileText),
			"Path: %s\nFile: %s -> %s\n",
			filePath, fileName, searchingWord
		);
		write(tempFileDescriptor, tempFileText, strlen(tempFileText));

	/* Opening file for searching (READ ONLY MODE) */
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

			/* Control word */
			if (currentChar == searchingWord[countArgv])
			{
				++countLetter;
				++countArgv;

				if (countLetter == strlen(searchingWord))
				{
					++totalWord;

					/* Results are writing to temp file. */
					snprintf(tempFileText,
						sizeof(tempFileText),
						"\t%d line, %d column found.\n",
						currentLineNumber,
						curentColumnNumber - (int)strlen(searchingWord)
					);
					write(tempFileDescriptor, tempFileText, strlen(tempFileText));

					/* Must be zero */
					countLetter = 0;
				}
			}
			else
			{
				countLetter = 0;
				countArgv = 0;
			}

		} /* end while */

		// End of file
		snprintf(tempFileText, sizeof(tempFileText), "------------------------------------\n");
		write(tempFileDescriptor, tempFileText, strlen(tempFileText));

		// Print screen
		printf("%s -> %d\n", fileName, totalWord);

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
			writePipe(tempFileDescriptor, pipeDescription);

			// Close temp file
			close(tempFileDescriptor);
		}

	} // end if else

	// Delete temp file
	unlink(tempFileName);

	return 0;

} // end function searchInFile

/***************************************************

	FUNCTION:	writeLogFile

	SUPPOSE:	Searching text in file

	COMMENT:

		Olcak olacak olacak o kadar..

****************************************************/
void writeLogFile(const char *results)
{
	int logFileHandle = open("gfD.log", O_CREAT | O_WRONLY | O_APPEND);
	write(logFileHandle, results, strlen(results));
	close(logFileHandle);

	return;
}

/***************************************************

	FUNCTION:	writePipe

	SUPPOSE:	Copy from temp file to pipe

	COMMENT:

		Olcak olacak olacak o kadar..

****************************************************/
void writePipe(const int tempFileDescription, const int pipeFileHandle)
{
	// Variable
	char tempText[BUFFER_SIZE];

	read(tempFileDescription, tempText, BUFFER_SIZE);
	write(pipeFileHandle, tempText, BUFFER_SIZE);

	return;
}

/***************************************************

	FUNCTION:	writeFifo

	SUPPOSE:	Copy text from pipe to fifo

	COMMENT:

		Olcak olacak olacak o kadar..

****************************************************/
/*
void writeFifo(const char *fifoDescription, const int a)
{

}
*/
