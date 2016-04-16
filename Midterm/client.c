#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>

#define BUFFER_SIZE 4096

int main()
{
	/* Fifo */
	char fifoBuffer[BUFFER_SIZE];
	/*
		We can try to open fifo file for server connection
	*/
	if (open("Connection", O_WRONLY) < 0)
	{
		printf("Server not open\nExiting..");
		exit(EXIT_FAILURE);
	}

	/*
		*
	*/
	if (write() < 0)
	{
		/* code */
	}


	return 1;
}