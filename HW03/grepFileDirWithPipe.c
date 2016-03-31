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


//
//   FUNCTION: void fileCheck(const char *, const char *)
//
//   PURPOSE: Checks the files.
//
//   COMMENTS:
//
//		Amaç dosyları kontrol etmek ve fork yapmaktır.
//		Her fork için pipe yapılır.
//		Önce fork işlemi yapılır daha sonra ise child process
//		dosya mı klasör mü olduğuna karar verir. Eğer dosya ise arama yapar ancak
//		folder ise bu sefer recursive yaparak fonksiyonu tekrar çağırır kendi alt processlerine
//		yeni path gönderir. İşlem aynı şekilde tekrarlanır.
//
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
	char currentChar[BUFFER_SIZE];
	ssize_t n;
	int fileOpenReadOnly;

	if ((fileOpenReadOnly = open(path, O_RDONLY)) < 0)
		perror("open");

	while((n = read(fileOpenReadOnly, currentChar, BUFFER_SIZE)) > 0)
		if (write(fd, currentChar, n) != n)
			perror("write");


	if (n < 0)
		perror("read");

	close(fileOpenReadOnly);

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
	char currentChar;

	while(read(fd, &currentChar, BUFFER_SIZE) > 0)
		printf("%c", currentChar);

	return;
}
