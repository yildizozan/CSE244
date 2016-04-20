#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#include "protocol.h"

int snprintf(char *, size_t, const char *, int);
int wait(void *);

void addClient(int queueNo, int pid);
void deleteClient(int queueNo, int pid);

/* Active connect computers */
static struct _response activeClientsTable[255];

int main(int argc, char *argv[])
{

	/* Argumans vaariables */
	float resulution = atof(argv[1]);
	int maxClient = atoi(argv[2]);

	/* Counters */
	int childProcessCount;
	int i;
	int tempi = 0;
	int exist = 0;
	int activeClients = 0;

	/* Protocol variables */
	struct _request request;
	struct _response response;
	struct _CAL CAL;
	struct _conn conn;

	/* Pipe variable */
	int pipeDescription[2];

	/* Fifo status */
	int fifoDescriptionMainConnection;
	int fifoDescriptionChild;
	char fifoSecureConnectionNameForClient[GTU_PRO_LEN];

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

	/*****************************

	Server prepearing

	*****************************/

	/* Reset active computer table */
	for (i = 0; i < maxClient; i++)
		activeClientsTable[i].pid = -1.0;

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

	/****************************

	Waiting clients

	*****************************/
	while (1)
	{
		/* Resets */
		exist = 0;

		printf("Client connet now: %d\n", activeClients);

		/*
		*	Response
		*	We'll waiting request protocol for continue..
		*/
		if (0 < (read(fifoDescriptionMainConnection, &request, sizeof(request)) != sizeof(request)))
			continue;

		/* Create a protocol */
		printf("PID: %ld olan bilgisayar baglanacak..\n", request.pid);
		snprintf(fifoSecureConnectionNameForClient, GTU_PRO_LEN, GTU_PRO_SEC, request.pid);

		/*
		*	Authentication
		*/
		for (i = 0; i < maxClient; ++i)
		{
			/*
			*	Tabloda zaten varmi diye bakiyoruz eger varsa
			*	Olumsuz yanit gönderiyoruz.
			*/
			if (activeClientsTable[i].pid == request.pid)
			{
				exist = 1;
				break;
			}

			if (response.identity == NULL)
				tempi = i;
		}

		/*
		*	Register
		*/
		if (exist == 0)
		{
			addClient(tempi, request.pid);
			strcpy(response.identity, fifoSecureConnectionNameForClient);
			response.status = 1;
			write(fifoDescriptionMainConnection, &response, sizeof(response));
		}
		else
		{
			deleteClient(i, request.pid);
			response.status = -1;
			write(fifoDescriptionMainConnection, &response, sizeof(response));
			continue;
		}

		/*
		Creating secure named pipe for comminucate
		*/
		printf("%s\n", fifoSecureConnectionNameForClient);
		if (mkfifo(fifoSecureConnectionNameForClient, 0666) < 0 && (errno != EEXIST))
		{
			printf("Error 2569\n");
			continue;
		}
		if (pipe(pipeDescription) < 0)
		{
			perror("Error 3659");
			continue;
		}
		if ((childPid = fork()) < 0)
		{
			perror("Error 4198");
			continue;
		}
		else
		{
			if (childPid) /* Parent process */
			{
				/* Server send info about client to child process*/
				close(pipeDescription[0]);
				write(pipeDescription[1], &request.pid, sizeof(request.pid));
				close(pipeDescription[1]);
			}
			else /* Child process */
			{
				long tempPid;

				/* Child process wait info about client from server*/
				close(pipeDescription[0]);
				read(pipeDescription[1], &request, sizeof(request));
				tempPid = request.pid;
				close(pipeDescription[1]);

				printf("%lu\n", request.pid);

				/* Open comminication named pipe */
				fifoDescriptionChild = open(fifoSecureConnectionNameForClient, O_RDWR);

				/* Child wait data of calculation from client */
				read(fifoDescriptionChild, &CAL, sizeof(CAL));

				/* Gelen CAL paketini hesapliyoruz*/
				/* TODO::*/

				while (1)
				{
					/* Wait any data*/
					read(fifoDescriptionChild, &request, sizeof(request));

					/* Control */
					if (request.pid == tempPid)
					{
						strcpy(conn.buffer, "Merhaba dunya");
						if (read(fifoDescriptionChild, &conn, sizeof(conn)) < 0)
							perror("Error 154");
					}
					else
					{
						printf("--- asd\n");
					}

					sleep(1);
				}


				close(fifoDescriptionChild);
				unlink(fifoSecureConnectionNameForClient);
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

	while (0 < (childPid = wait(NULL)))
	{
		printf("-Chil: %ld completed. Total %d\n", (long)getpid(), childProcessCount++);
	}

	return 1;
}

void addClient(int queueNo, int pid)
{
	char identity[GTU_PRO_LEN];
	snprintf(identity, GTU_PRO_LEN, GTU_PRO_SEC, pid);
	strcpy(activeClientsTable[queueNo].identity, identity);
	activeClientsTable[queueNo].pid = pid;
	activeClientsTable[queueNo].status = 1;
}

void deleteClient(int queueNo, int pid)
{
	activeClientsTable[queueNo].pid = 0;
}