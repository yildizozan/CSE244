#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define BUFFER_SIZE 4096

int main()
{
	/* Fifo */
	int fifoFileDescription;
	char fifoBuffer[BUFFER_SIZE];
	/*
		We can try to open fifo file for server connection
	*/
	if ((fifoFileDescription = open("Connection", O_WRONLY)) < 0)
	{
		printf("Server not open\nExiting..");
		exit(EXIT_FAILURE);
	}

	printf("Your message to server: ");
	scanf("%s", fifoBuffer);

	/*
		*
	*/
	if (write(fifoFileDescription, fifoBuffer, BUFFER_SIZE) < 0)
	{
		printf("Fifo gönderilemedi..\n");
	}

	close(fifoFileDescription);


	return 1;
}