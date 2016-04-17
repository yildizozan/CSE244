#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#include "protocol.h"

#define BUFFER_SIZE 4096

int snprintf(char *, size_t, const char *, int);

int main()
{
	/* Protocol variables */
	struct _request request;
	struct _response response;

	/* Fifo variables */
	int fifoDescriptionMainConnection;
	int fifoDescriptionNewSecureConnection;
	char fifoBuffer[BUFFER_SIZE];
	char *fifoNewSecureConnectionNameForServer;

	/* Control variables */
	int status;

	/* Control printf */
	printf("-%d\n", (int)getpid());

	/*
		We can try to open fifo file for server connection
	*/
	if ((fifoDescriptionMainConnection = open(GTU_PRO_NAM, O_RDWR)) < 0)
	{
		printf("Server not open\nExiting..");
		close(fifoDescriptionMainConnection);
		exit(EXIT_FAILURE);
	}
	else
	{
		/*
			Create request protocol
			Client send pid number to server.
			Server create new fifo with client pid number.
		*/
		request.pid = (int)getpid();
		if (write(fifoDescriptionMainConnection, &request, sizeof(request)) < 0)
			perror("Error code 849:");

		/*
			Try to connect server
		*/
		if (read(fifoDescriptionMainConnection, &response, sizeof(response)) < 0)
		{
			perror("Error try again code 516:");
			exit(EXIT_FAILURE);
		}
	}

	printf("-- ID: %d - SEC: %s\n", response.identityNo, response.identity);

	/*
		Response protocol control
	*/
	snprintf(fifoNewSecureConnectionNameForServer, GTU_PRO_LEN, GTU_PRO_SEC, (int)getpid());
	if (strcmp(fifoNewSecureConnectionNameForServer, response.identity) == 0)
	{

		if ((fifoDescriptionNewSecureConnection = open(fifoNewSecureConnectionNameForServer, O_RDWR)) < 0)
			perror("Error code 342:");

		if (0 < read(fifoDescriptionNewSecureConnection, fifoBuffer, BUFFER_SIZE))
		{
		printf("%s", fifoBuffer);
		};

	} /* end if protocol control and server connection */




	close(fifoDescriptionNewSecureConnection);
	close(fifoDescriptionMainConnection);

	return 1;
}