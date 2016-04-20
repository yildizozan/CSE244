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
	/* Counters */
	int tryConnection = 0;
	/* Protocol variables */
	struct _request request;
	struct _response response;
	struct _conn conn;

	/* Fifo variables */
	int fifoDescriptionMainConnection;
	int fifoDescriptionNewSecureConnection;
	char fifoBuffer[BUFFER_SIZE];
	char *fifoNewSecureConnectionNameForServer;

	/* Control variables */

	/*
	*	We can try to open fifo file for server connection
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
		*	1- Fill request
		*	2- Create protocol and add client pid in protocol
		*	3- Send request (send pid)
		*	4- Server response is pid and protocolddd
		*/
		request.pid = getpid();
		snprintf(fifoNewSecureConnectionNameForServer, GTU_PRO_LEN, GTU_PRO_SEC, request.pid);

		while (strcmp(fifoNewSecureConnectionNameForServer, response.identity) != 0)
		{
			if (write(fifoDescriptionMainConnection, &request, sizeof(request)) < 0)
				perror("Error code 849:");

			if (read(fifoDescriptionMainConnection, &response, sizeof(response)) < 0)
			{
				perror("Error try again code 516:");
				exit(EXIT_FAILURE);
			}

			if (tryConnection == 5)
				printf("Connectin error to server.\n");

			if (tryConnection == 10)
				printf("Cannot connect to server.\n");

			printf("Tekrar bağlanmaya çalışılacak..\n");
			tryConnection++;
			sleep(1);
		}

		printf("Connection OK!\n");

	}

	/*
	*	Connection OK!
	*/
	if ((fifoDescriptionNewSecureConnection = open(fifoNewSecureConnectionNameForServer, O_RDWR)) < 0)
		perror("Error code 342:");

	/* Sending CAL */


	while (1)
	{

		/* Response */
		read(fifoDescriptionNewSecureConnection, &conn, sizeof(conn));

		if (conn.status == 1)
			printf("%s\n", conn.buffer);
		else
		{
			printf("Tekrar deneyecek\n");
			sleep(3);
		}
	}

	close(fifoDescriptionMainConnection);
	close(fifoDescriptionNewSecureConnection);

	return 1;
}