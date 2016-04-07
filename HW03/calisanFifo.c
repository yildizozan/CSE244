#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <unistd.h> 
#include <sys/stat.h> 

#define BUFFER_SIZE 256

#define READ_ONLY (O_RDONLY) 
#define WRITE_ONLY (O_WRONLY)

int main(int argc, char const *argv[])
{
	int fifoDescripton;
	char fifoName[50];
	snprintf(fifoName, sizeof(fifoName), "%d", (int)getpid());

	size_t pid;


	if ((mkfifo(fifoName, READ_ONLY) < 0) || ((pid = fork()) < 0))
	{
		perror("fork");
		perror("mkfifo");
		exit(EXIT_FAILURE);
	}
	else
	{
		if (pid == 0)
		{
			printf("I am CHILD\n");
			if ((fifoDescripton = open(fifoName, WRITE_ONLY)) < 0)
			{

				perror("open");
				exit(EXIT_FAILURE);
			}
			else
			{
				write(fifoDescripton, "Hi", sizeof("Hi"));
				close(fifoDescripton);
			}
		}
		else
		{
			printf("I am PARENT\n");
			if ((fifoDescripton = open(fifoName, READ_ONLY)) < 0)
			{
				perror("open");
				exit(EXIT_FAILURE);
			}
			else
			{
				char buf[BUFFER_SIZE];
				read(fifoDescripton, buf, BUFFER_SIZE);
    			printf("Received: %s\n", buf);
    			close(fifoDescripton);
			}
		}
	}

	return 0;
}