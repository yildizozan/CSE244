#include "protocol.h"

int main(int argc, char const *argv[])
{
	int fdMainConnRequest;
	int fdMainConnResponse;

	char buffer[BUFFER_SIZE];

	/* create the FIFO (named pipe) */
	mkfifo(GTU_PRO_REQ, 0666);
	mkfifo(GTU_PRO_RES, 0666);

	/* open, read, and display the message from the FIFO */
	fdMainConnRequest = open(GTU_PRO_REQ, O_RDONLY);
	fdMainConnResponse = open(GTU_PRO_RES, O_WRONLY);

	printf("Server ON.\n");

	while (1)
	{
		read(fdMainConnRequest, buffer, BUFFER_SIZE);

		if (strcmp("exit", buffer) == 0)
		{
			printf("Server OFF.\n");
			break;
		}

		else if (strcmp("", buffer) != 0)
		{
			printf("%s\n", buffer);
			printf("Received: %s\n", buffer);
			printf("Sending back...\n");
			strcpy(buffer, "1");
			write(fdMainConnResponse, buffer, BUFFER_SIZE);
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

