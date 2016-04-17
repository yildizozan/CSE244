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
int wait(void *);


int main(int argc, char *argv[])
{
	/* Protocol variables */
	struct _request request;
	struct _response response;
	int identity = 0;

	/* Argumans vaariables */
	float resulution = atof(argv[1]);
	int maxClient = atoi(argv[2]);

	/* Fifo status */
	int fifoFileDescriptionMain;
	int fifoFileDescriptionChild;
	char fifoBuffer[BUFFER_SIZE];
	char *fifoFileNewConnection;

	/* Child process variables */
	pid_t childPid;

	/*
	if (argc != 3)
	{
	printf("Wrong parameters!\n");
	printf("Usage: ./server <(int)resulution> <(int)maxClient>\n");
	exit(EXIT_FAILURE);
	}
	*/
	/* Control */
	printf("--%f - %d\n", resulution, maxClient);

	/*
	We'll try to create fifo file for connection
	if there is a fifo connection file, exiting for security
	*/
	if (mkfifo(GTU_PRO_NAM, 0666) < 0 && (errno != EEXIST))
	{
		printf("Cannot create 'Connection' fifo file..\n");
		printf("We'll exiting..\n");
		exit(EXIT_FAILURE);
	}

	if ((fifoFileDescriptionMain = open(GTU_PRO_NAM, O_RDWR)) < 0)
	{
		printf("Cannot open fifo file, please try again\n");
		printf("We'll exiting..\n");
		exit(EXIT_FAILURE);
	}
	else
		printf("Server working and waiting client(s)..\n");

	/*
	Waiting clients
	*/
	while (1)
	{
		printf("Client connet now: %d\n", identity);
		/*
		We'll waiting request protocol for continue..
		*/
		if (read(fifoFileDescriptionMain, &request, sizeof(request)) != sizeof(request))
			continue;

		/*
		Close server commant
		*/
		if (strcmp(fifoBuffer, "exit") == 0)
		{
			printf("U r fuck'in admin!\nSh.. Shu.. Shut down server..\n");
			exit(EXIT_SUCCESS);
		}

		printf("Client pid: %d connecting..\n", request.pid);



		/* Create new identity */
		identity++;
		response.identityNo = identity;
		write(fifoFileDescriptionMain, &response, sizeof(response));

		printf("%s\n", fifoBuffer);

		/*
		Creating secure named pipe for comminucate
		*/
		snprintf(fifoBuffer, GTU_PRO_LEN, GTU_PRO_SEC, request.pid);
		strcpy(fifoFileNewConnection, fifoBuffer);
		if ((mkfifo(fifoFileNewConnection, 0666) < 0 && (errno != EEXIST)) || (childPid = fork()) < 0)
		{
			perror("Error: ");
			continue;
		}
		else
		{
			/*
			** Child process
			*/

			{
				chil open(fifoFileNewConnection, O_RDWR);
				strcpy(fifoBuffer, "Merhaba ben child!\n");
				write(fifoFileDescriptionChild, fifoBuffer, BUFFER_SIZE);
				close(fifoFileDescriptionChild);

				identity--;
				exit(EXIT_SUCCESS);
			}

		} /* end if else open */

	} /* end while */

	  /*
	  Close and delete fifo files
	  */
	close(fifoFileDescriptionMain);
	unlink("Connection");

	wait(NULL);

	return 1;
}