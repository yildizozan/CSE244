#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define BUFFER_SIZE 1

void fileCheck(const char *path, const char *text);

// Pipe functions
void pipeWriting(const int fd, const char *path);
void pipeReading(const int fd);

int main(int argc, char const *argv[])
{
	/*
	if (argc != 3)
	{
		printf("Usage: ./grepFromDir [directory] [searching text}\n");
		exit(EXIT_FAILURE);
	}
	*/

	fileCheck(argv[1], argv[2]);

	return 0;
}

void fileCheck(const char *path, const char *text)
{
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
		}
	}
}

//
//   FUNCTION: pipeWriting(const int, const char *)
//
//   PURPOSE: Writing with pipe
//
//   COMMENTS:
//
//		Aldığı path argümanı ile ilgili dosyayı READONLY modunda açar.
//		Daha sonra açmış olduğu bu dosyadan BUFFER_SIZE miktarı kadar okur.
//		BUFFER_SIZE miktarı default olarak 1 olarak ayarladım.
//		Her bir byte pipe dosyasına yazılır ve dosya kapatılır.
//
void pipeWriting(const int fd, const char *path)
{
	char bufferSize[BUFFER_SIZE];
	ssize_t n;
	int fdf;

	if ((fdf = open(path, O_RDONLY)) < 0)
		perror("open");

	while((n = read(fdf, bufferSize, BUFFER_SIZE)) > 0)
		if (write(fd, bufferSize, n) != n)
			perror("write");

	if (n < 0)
		perror("read");

	close(fdf);
	
	return;
}

//
//   FUNCTION: pipeReading(const int)
//
//   PURPOSE: Reading with pipe
//
//   COMMENTS:
//
//		Dosyadan okumayı sağlar pipe ile 
//
void pipeReading(const int fd)
{
	char bufferSize[BUFFER_SIZE];
	ssize_t n;

	while((n  = read(fd, bufferSize, BUFFER_SIZE)) > 0)
		if (write(STDOUT_FILENO, bufferSize, n) < 0)
			perror("pipe");

	if (n < 0)
		perror("read");

	return;
}
