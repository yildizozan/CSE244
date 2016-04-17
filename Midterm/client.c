#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>

#define BUFFER_SIZE 4096

int main()
{
	/*
	Fifo variables
	*/
	int fifoFileDescription;
	char fifoBuffer[BUFFER_SIZE];

	/* Control printf */
	printf("-%d\n", (int)getpid());

	/*
	We can try to open fifo file for server connection
	*/
	if ((fifoFileDescription = open("Connection", O_RDWR)) < 0)
	{
		printf("Server not open\nExiting..");
		exit(EXIT_FAILURE);
	}

	/*
	Client send pid number to server.
	Server create new fifo with client pid number.
	*/
	sprintf(fifoBuffer, "%d", (int)getpid());
	if (write(fifoFileDescription, fifoBuffer, sizeof(fifoBuffer)) < 0)
	{
		printf("Not connecting..\n");
		exit(EXIT_FAILURE);
	}

	if (read(fifoFileDescription, fifoBuffer, sizeof(fifoBuffer)) < 0)
	{
		printf("Server busy, error code: 659..\nExiting..\n");
		exit(EXIT_FAILURE);
	}

	/* Control printf */
	printf("--%s\n", fifoBuffer);

	/*
	Server OK message
	*/
	if (strcmp(fifoBuffer, "OK!") != 0)
	{
		printf("Yannis\n");
		exit(EXIT_FAILURE);
	}

	/* Close connection fifo */
	close(fifoFileDescription);



	return 1;
}