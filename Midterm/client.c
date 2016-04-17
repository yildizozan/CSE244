#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#include "protocol.h"

#define BUFFER_SIZE 4096

int main()
{
	/* Protocol variables */
	struct _request request;
	struct _response response;

	/* Fifo variables */
	int fifoFileDescriptionMain;
	int fifoFileDescriptionSafeConn;
	char fifoBuffer[BUFFER_SIZE];
	char *fifoFileNewConnection;

	/* Control printf */
	printf("-%d\n", (int)getpid());

	/*
	We can try to open fifo file for server connection
	*/
	if ((fifoFileDescriptionMain = open(GTU_PRO_NAM, O_RDWR)) < 0)
	{
		printf("Server not open\nExiting..");
		close(fifoFileDescriptionMain);
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
		if (write(fifoFileDescriptionMain, &request, sizeof(request)) < 0)
			perror("Error code 849:");

		if (write(fifoFileDescriptionMain, &response, sizeof(response)) < 0)
			perror("Error code 516:");
	}

printf("-516: %d\n", response.identityNo);

	snprintf(fifoFileNewConnection, GTU_PRO_LEN, GTU_PRO_SEC, (int)getpid());
	if ((fifoFileDescriptionSafeConn = open(fifoBuffer, O_RDWR)) < 0)
		perror("Error code 342:");

	read(fifoFileDescriptionSafeConn, fifoBuffer, sizeof(BUFFER_SIZE));





	close(fifoFileDescriptionMain);

	return 1;
}