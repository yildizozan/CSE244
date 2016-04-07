/* 
 * File:   Source.c
 * Author: Ozan Yıldız
 *
 * Created on April 2, 2016, 11:03 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

// Buffers
#define BUFFER_SIZE 1
#define MAX_PATH 256
#define MAX_TEXT_LENGTH 2048

// Files Attributes
#define READ_ONLY (O_RDONLY) 
#define WRITE_ONLY (O_WRONLY)

void openDirectory(const char *, const char *);
int searchInFile(const char *, const char*, const char *, int);

void writingLog(const char*);

// Pipe functions
void pipeWriting(const int, const char *);
void pipeReading(const int);

int main(int argc, char const *argv[])
{
	/*
	if (argc != 3)
	{
	printf("Usage: ./grepFromDir [directory] [searchInFile text}\n");
	exit(EXIT_FAILURE);
	}
	*/

	openDirectory(argv[1], argv[2]);

	return 0;
}

//
//   FUNCTION:	openDirectory
//
//   PURPOSE:	Checks the files.
//
//   COMMENTS(TR):
//
//		Amaç dosyları kontrol etmek ve fork yapmaktır.
//		Her fork için pipe yapılır.
//		Önce fork işlemi yapılır daha sonra ise child process
//		dosya mı klasör mü olduğuna karar verir. Eğer dosya ise arama yapar ancak
//		folder ise bu sefer recursive yaparak fonksiyonu tekrar çağırır kendi alt processlerine
//		yeni path gönderir. İşlem aynı şekilde tekrarlanır.
//
void openDirectory(const char *path, const char *text)
{
	// Folder variables
	DIR *directory;
	struct dirent *entry;

	// Pipe and fork variable
	int fileDescription[2];
	pid_t childPid;

	// Fifo variables
	int fifoDescripton;
	char fifoName[50];
	snprintf(fifoName, sizeof(fifoName), "%d", (int)getpid());

	if ((mkfifo(fifoName, READ_ONLY) < 0))
	{
		perror("mkfifo");
		exit(EXIT_FAILURE);
	}

	// Start to opendir path
	if ((directory = opendir(path)) == NULL)
	{
		perror("opendir");
		exit(EXIT_FAILURE);
	}
	else
	{
		while ((entry = readdir(directory)) != NULL)
		{ 

			if ((pipe(fileDescription) < 0) || ((childPid = fork()) < 0))
			{
				perror("pipe");
				perror("fork");
				exit(EXIT_FAILURE);
			}
			else if (childPid) // Parent proces
			{
printf("Parent start\n");

				// Wait all process
				wait(NULL);

				if ((fifoDescripton = open(fifoName, READ_ONLY)) < 0)
				{
printf("fifo error\n");
					perror("open");
					exit(EXIT_FAILURE);
				}
				else if (fifoDescripton == 0)
				{
				printf("Parent start\n");
					close(fifoDescripton);
				}
				else
				{
printf("fifo succes\n");
					char buf[BUFFER_SIZE];
					read(fifoDescripton, buf, BUFFER_SIZE);
	    			printf("Received: %s\n", buf);
	    			close(fifoDescripton);
	    			unlink(fifoName);
				}

				char readText[MAX_TEXT_LENGTH];

				close(fileDescription[1]);
				read(fileDescription[0], &readText, sizeof(readText));

				close(fileDescription[0]);
				writingLog(readText);
			}
			else // Child process
			{
				if (strcmp(entry->d_name, "..") == 0 || strcmp(entry->d_name, ".") == 0)
					exit(EXIT_FAILURE);

				// Control: Is this folder?
				if (entry->d_type == DT_DIR)
				{
					printf("DT_DIR start\n");

					// Fifo write
					if ((fifoDescripton = open(fifoName, WRITE_ONLY)) < 0)
					{
						perror("open");
						exit(EXIT_FAILURE);
					}
					else if (fifoDescripton == 0)
					{
						close(fifoDescripton);
					}
					else
					{
						char asd[] = "Hello";
						write(fifoDescripton, asd, sizeof(asd));
						close(fifoDescripton);
					}

					// Create new folder path
					char newFolderPath[MAX_PATH];
					strcpy(newFolderPath, path);
					strcat(newFolderPath, "/");
					strcat(newFolderPath, entry->d_name);


					// openDirectory new path
					openDirectory(newFolderPath, text);
					exit(EXIT_SUCCESS);
				}

				// Control: Is this file?
				else if (entry->d_type == DT_REG)
				{

					printf("DT_REG start\n");
					// Pipe
					close(fileDescription[0]);

					// We search text in file
					searchInFile(path, entry->d_name, text, fileDescription[1]);

					close(fileDescription[1]);
					exit(EXIT_SUCCESS);
				}
			
			}
		} // end while

	} // end if else

} // end function

//
//   FUNCTION:	searchInFile
//
//   PURPOSE:	Search file and send with pipe
//
//   COMMENTS(TR):
//
//		Aldığı filePath ve fileName 
//		Aldığı filePath argümanı ile ilgili dosyayı READONLY modunda açar.
//		Daha sonra açmış olduğu bu dosyadan BUFFER_SIZE miktarı kadar okur.
//		BUFFER_SIZE miktarı default olarak 1 olarak ayarladım.
//		Her bir byte pipe dosyasına yazılır ve dosya kapatılır.
//
int searchInFile(
	const char* filePath, 
	const char* fileName, 
	const char *searchInFileText, 
	int fileDescription)
{
	// Create new folder path
	char newPath[MAX_PATH];
	strcpy(newPath, filePath);
	strcat(newPath, "/");
	strcat(newPath, fileName);

	// Control
	printf("%s/%s - %d - Parent: %d\n", filePath, fileName, getpid(), getppid());

	// Variables
	int currentLineNumber = 1,		// currentLineNumber is line counter
		curentColumnNumber = 1,		// curentColumnNumber is column counter
		countLetter = 0,			// countLetter need to while loop
		totalWord = 0;				// totalWord find searchInFile text in the file

	char currentChar;

	// Opening file (READ ONLY MODE)
	int openFileForReading = open(newPath, O_RDONLY);

	// Opening temp file for result
	char tempfileName[MAX_TEXT_LENGTH];
	snprintf(tempfileName, sizeof(tempfileName), "%d.txt", getpid());
	int tempFileForWriting = open(tempfileName, O_CREAT | O_WRONLY | O_APPEND);

		// Create result header text for temp file
		char tempResultText[MAX_TEXT_LENGTH];
		snprintf(
		tempResultText, 
		sizeof(tempResultText),
		"Path: %s\n%s word in %s file\n",
			filePath, 
			searchInFileText, 
			fileName
		);
		write(tempFileForWriting, tempResultText, strlen(tempResultText));


	// We're starting to read file
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
			if (currentChar == searchInFileText[countArgv])
			{
				++countLetter;
				++countArgv;

				if (countLetter == strlen(searchInFileText))
				{
					++totalWord;

					// Create result string for write temp file
					snprintf(
						tempResultText, 
						sizeof(tempResultText),
						"\t%d line, %zu column found.\n",
							currentLineNumber, 
							curentColumnNumber - strlen(searchInFileText)
						);

					// We're writing result string in temp file
					write(tempFileForWriting, tempResultText, strlen(tempResultText));

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

		// Send pipe all results
		char reagent[] = "----------\n";
		write(tempFileForWriting, reagent, strlen(reagent));
		close(tempFileForWriting);

		// Send pipe all result
		char asd[MAX_TEXT_LENGTH];
		int fd = open(tempfileName, O_RDONLY);
		if (read(tempFileForWriting, asd, MAX_TEXT_LENGTH) < 0)
			perror("error");

		write(fileDescription, asd, sizeof(asd) + 1);
		close(fd);

 		// If no results, delete temp file
		if (totalWord == 0)
			unlink(tempfileName);

		// Close reading file
		close(openFileForReading);

	}

	return 0;
}

//
//   FUNCTION:	writingLog
//
//   PURPOSE:	Parent write all result 
//
//   COMMENTS(TR):
//
//		main() fonksiyondaki parent pipe ve fifolardan gelen herşeyi log file yazar
//
void writingLog(const char* text)
{
	int logFileForWriting;
	
	if ((logFileForWriting = open("gfD.log", O_CREAT | O_WRONLY | O_APPEND)) < 0)
		perror("open");

	write(logFileForWriting, text, strlen(text));

	close(logFileForWriting);
}


/* 
	NOTES

	Pipe
		-

	Fifo
		- Fifo için child yazarken parent pid değerine göre yazar
		- Parent ise okurken kendi pid değerine göre okur.


*/