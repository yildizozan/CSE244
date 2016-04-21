#include "protocol.h"

static struct _EXCP activeClientTable[255];

int main(int argc, char const *argv[])
{
	/* Argumans vaariables */
	/*	float resulution = atof(argv[1]);*/
	int maxClients = atoi(argv[2]);

	/* Counters */
	int i;

	/* Control variables */
	int disconnect;

	/* Buffers */
	char buffer[BUFFER_SIZE];

	/* Fifo variables */
	int fdMainConnRequest;
	int fdMainConnResponse;

	printf("--Control: %d\n", maxClients);

	/*************************************

	*	PREPARE SERVER

	*************************************/

	/* Table reset */
	for (i = 0; i < maxClients; ++i)
		activeClientTable[i].pid = 0;

	/* Creating fifos */
	if ((mkfifo(GTU_PRO_REQ, 0666)) && (errno != EEXIST))
	{
		perror("Error 659");
		exit(EXIT_FAILURE);
	}
	if ((mkfifo(GTU_PRO_REQ, 0666)) && (errno != EEXIST))
	{
		perror("Error 236");
		exit(EXIT_FAILURE);
	}


	/* open, read, and display the message from the FIFO */
	fdMainConnRequest = open(GTU_PRO_REQ, O_RDONLY);
	fdMainConnResponse = open(GTU_PRO_RES, O_WRONLY);

	printf("Server ON.\n");

	while (1)
	{
		/* Control resets */
		disconnect = 0;

		/*
		*	Waiting a client
		*/
		read(fdMainConnRequest, buffer, BUFFER_SIZE);

		if (strcmp("exit", buffer) == 0)
		{
			printf("Server OFF.\n");
			break;
		}

		else if (strcmp("", buffer) != 0)
		{
			/*
			*	Client zaten bagli mi diye kontrol ediyoruz
			*	Eger zaten bagli ise disconnect yollayacagiz illegal bir durum
			*/
			for (i = 0; i < maxClients; ++i)
				if (activeClientTable[i].pid == atoi(buffer))
				{
					sprintf(buffer, "-1");
					write(fdMainConnResponse, buffer, BUFFER_SIZE);
					disconnect = 1;
					break;
				}

			/*
			*	Client bagli ise bos geciyoruz
			*	Eger bagli degilse register edecegiz
			*/
			if (disconnect == 1)
			{
				continue;
			}
			else
			{
				printf("%s\n", buffer);
				printf("Received: %s\n", buffer);
				printf("Sending back...\n");
				sprintf(buffer, "1");
				write(fdMainConnResponse, buffer, BUFFER_SIZE);
			}


		}

		/* clean buffer from any data */
		memset(buffer, 0, sizeof(buffer));
	}

	close(fdMainConnRequest);
	close(fdMainConnResponse);

	unlink(GTU_PRO_REQ);
	unlink(GTU_PRO_RES);
	return 0;
}

