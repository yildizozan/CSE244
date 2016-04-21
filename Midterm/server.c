#include "protocol.h"

int main(int argc, char const *argv[])
{
	int fdMainConnRequest;
	int fdMainConnResponse;

	char buf[BUFFER_SIZE];

	/* create the FIFO (named pipe) */
	mkfifo(GTU_PRO_REQ, 0666);
	mkfifo(GTU_PRO_RES, 0666);

	/* open, read, and display the message from the FIFO */
	fdMainConnRequest = open(GTU_PRO_REQ, O_RDONLY);
	fdMainConnResponse = open(GTU_PRO_RES, O_WRONLY);

	printf("Server ON.\n");

	while (1)
	{
		read(fdMainConnRequest, buf, BUFFER_SIZE);

		if (strcmp("exit", buf) == 0)
		{
			printf("Server OFF.\n");
			break;
		}

		else if (strcmp("", buf) != 0)
		{
			printf("Received: %s\n", buf);
			printf("Sending back...\n");
			write(fdMainConnResponse, buf, BUFFER_SIZE);
		}

		/* clean buf from any data */
		memset(buf, 0, sizeof(buf));
	}

	close(fdMainConnRequest);
	close(fdMainConnResponse);

	unlink(GTU_PRO_REQ);
	unlink(GTU_PRO_RES);
	return 0;
}