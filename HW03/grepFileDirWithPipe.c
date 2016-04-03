#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>

#define BUFFER_SIZE 1
#define MAX_PATH 256

void openDirectory(const char *, const char *);
int searchFile(const char *, const char *);

// Pipe functions
void pipeWriting(const int, const char *);
void pipeReading(const int);

int main(int argc, char const *argv[])
{
	/*
	if (argc != 3)
	{
	printf("Usage: ./grepFromDir [directory] [searchFile text}\n");
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

	if ((directory = opendir(path)) == NULL)
	{
		perror("opendir");
		exit(EXIT_FAILURE);
	}
	else
	{
		while ((entry = readdir(directory)) != NULL)
		{
			
			if (strcmp(entry->d_name, "..") == 0 || strcmp(entry->d_name, ".") == 0)
				continue;
			
			// Control: Is this folder?
			if (entry->d_type == DT_DIR)
			{
				// Create new folder path
				char newFolderPath[MAX_PATH];
				strcpy(newFolderPath, path);
				strcat(newFolderPath, "/");
				strcat(newFolderPath, entry->d_name);

				// openDirectory new path
				openDirectory(newFolderPath, text);

			}

			// Control: Is this file?
			if (entry->d_type == DT_REG)
			{
				// Create file path
				char newFilePath[MAX_PATH];
				strcpy(newFilePath, path);
				strcat(newFilePath, "/");
				strcat(newFilePath, entry->d_name);

				// searchFile in file
				searchFile(newFilePath, text);

				//exit(EXIT_SUCCESS);
			}
		}
	}

	int fileDescription[2];

	if (pipe(fileDescription) < 0)
	{
		perror("pipe");
		exit(EXIT_FAILURE);
	}
	else
	{
		// Create Process
		pid_t pid = fork();

		if (pid < 0) // Process can't create
		{
			perror("fork");
			exit(EXIT_FAILURE);
		}
		else if (pid == 0) // Child proces
		{


			close(fileDescription[0]);
			pipeWriting(fileDescription[1], path);
			close(fileDescription[1]);
			exit(EXIT_SUCCESS);
		}
		else // Parent process
		{

			close(fileDescription[1]);
			pipeReading(fileDescription[0]);
			close(fileDescription[0]);
			exit(EXIT_SUCCESS);
		}
	}
}

//
//   FUNCTION:	searchFile
//
//   PURPOSE:	Writing with pipe
//
//   COMMENTS(TR):
//
//		Aldığı path argümanı ile ilgili dosyayı READONLY modunda açar.
//		Daha sonra açmış olduğu bu dosyadan BUFFER_SIZE miktarı kadar okur.
//		BUFFER_SIZE miktarı default olarak 1 olarak ayarladım.
//		Her bir byte pipe dosyasına yazılır ve dosya kapatılır.
//
int searchFile(const char *filePath, const char *searchFileText)
{
	/* Test */
	printf("%s\n", filePath);

	// Variables
	int currentLineNumber = 1,	// currentLineNumber is line counter
		curentColumnNumber = 1,		// curentColumnNumber is column counter
		countLetter = 0,			// countLetter need to while loop
		totalWord = 0;				// totalWord find searchFile text in the file

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
			if (currentChar == searchFileText[countArgv])
			{
				++countLetter;
				++countArgv;

				if (countLetter == strlen(searchFileText))
				{
					++totalWord;

					// Write a file
					FILE *openFileForWriting = fopen("gfD.log", "a+");
					fprintf(openFileForWriting, "%s file => %s word, %d line and %zu column found.\n", filePath, searchFileText, currentLineNumber, curentColumnNumber - strlen(searchFileText));
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

//
//   FUNCTION:	pipeWriting
//
//   PURPOSE:	Writing with pipe
//
//   COMMENTS(TR):
//
//		Aldığı path argümanı ile ilgili dosyayı READONLY modunda açar.
//		Daha sonra açmış olduğu bu dosyadan BUFFER_SIZE miktarı kadar okur.
//		BUFFER_SIZE miktarı default olarak 1 olarak ayarladım.
//		Her bir byte pipe dosyasına yazılır ve dosya kapatılır.
//
void pipeWriting(const int pipeFile, const char *path)
{
	char currentChar;

	// Open file
	int openFileForReading;

	if ((openFileForReading = open(path, O_RDONLY)) < 0)
		perror("open");

	while (read(openFileForReading, &currentChar, BUFFER_SIZE) > 0)
		write(pipeFile, &currentChar, BUFFER_SIZE);

	close(openFileForReading);

	return;
}

//
//   FUNCTION:	pipeReading
//
//   PURPOSE:	Reading with pipe
//
//   COMMENTS(TR):
//
//		Dosyadan okumayı sağlar pipe ile 
//
void pipeReading(const int pipeFile)
{
	char currentChar;
	int openFileForWriting;

	//openFileForWriting = open("HW03.c", O_CREAT | O_WRONLY | O_APPEND);

	if ((openFileForWriting = open("HW03.cpp", O_CREAT | O_WRONLY | O_APPEND)) < 0)
		perror("open");

	while (read(pipeFile, &currentChar, BUFFER_SIZE) > 0)
		if (write(openFileForWriting, &currentChar, BUFFER_SIZE) < 0)
			perror("write");

	close(openFileForWriting);

	return;
}

/* THE END */