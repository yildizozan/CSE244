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
	int fifoDescriptionMainConnection;
	int fifoDescriptionChild;
	char fifoBuffer[BUFFER_SIZE];
	char fifoSecureConnectionNameForClient[BUFFER_SIZE];

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

	if ((fifoDescriptionMainConnection = open(GTU_PRO_NAM, O_RDWR)) < 0)
	{
		printf("Cannot open fifo file, please try again\n");
		printf("We'll exiting..\n");
		exit(EXIT_FAILURE);
	}
	else
		printf("Server working and waiting client(s)..\n");

	/*
		Waiting clients *****************************
	*/
	while (1)
	{

		printf("Client connet now: %d\n", identity);

		/*
			Response
			We'll waiting request protocol for continue..
		*/
		if (0 < (read(fifoDescriptionMainConnection, &request, sizeof(request)) != sizeof(request)))
			continue;

		printf("Client pid: %d connecting..\n", request.pid);

		/*
			Creating secure named pipe for comminucate
		*/
		snprintf(fifoSecureConnectionNameForClient, GTU_PRO_LEN, GTU_PRO_SEC, request.pid);
printf("%s\n", fifoSecureConnectionNameForClient);
		if (mkfifo(fifoSecureConnectionNameForClient, 0666) < 0 && (errno != EEXIST))
		{
			printf("Connot connect!\n");
			continue;
		}

		if ( (childPid = fork()) < 0)
		{
			perror("Error 4198");
			continue;
		}
		else
		{
			if (childPid) /* Parent process */
			{
				/* Create new identity */
				identity++;
				response.identityNo = identity;
				strcpy(response.identity, fifoSecureConnectionNameForClient);
				write(fifoDescriptionMainConnection, &response, sizeof(response));
				wait(NULL);
			}
			else /* Child process */
			{

				fifoDescriptionChild = open(fifoSecureConnectionNameForClient, O_RDWR);
				if (write(fifoDescriptionChild, "Welcome. I'm chil process\n", 
					sizeof("Welcome. I'm chil process\n")) < 0)
				{
					perror("Error 154");
				}

				exit(EXIT_SUCCESS);
			}

		} /* end if else open */

	} /* end while */

	/*
	Close and delete fifo files
	*/
	close(fifoDescriptionChild);
	close(fifoDescriptionMainConnection);
	unlink("Connection");

	int childProcessCount;
	while (0 < (childPid = wait(NULL)))
	{
		printf("-Chil: %d completed. Total %d\n", (int)getpid(), childProcessCount++);
	}

	return 1;
}