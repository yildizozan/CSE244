#include "protocol.h"

int main(int argc, char const *argv[])
{
	/* Protocols */
	struct _EXCP EXCP;

	/* Buffer */
	char buf[BUFFER_SIZE];

	/* Fifo variables */
	int fdMainConnRequest;
	int fdMainConnResponse;

	/* create the FIFO (named pipe) */
	mkfifo(GTU_PRO_REQ, 0666);
	mkfifo(GTU_PRO_RES, 0666);

	/* open, read, and display the message from the FIFO */
	fdMainConnRequest = open(GTU_PRO_REQ, O_RDONLY);
	fdMainConnResponse = open(GTU_PRO_RES, O_WRONLY);

	printf("Server ON.\n");

	while (1)
	{
		if (read(fdMainConnRequest, &EXCP, sizeof(struct _EXCP)) != sizeof(struct _EXCP));
		continue;
		printf("Gelen:\n%ld\n%s\n%d\n%s\n------\n", (long)EXCP.pid, EXCP.identity, EXCP.status, EXCP.message);


		write(fdMainConnResponse, &EXCP, sizeof(struct _EXCP));
		printf("Giden:\n%ld\n%s\n%d\n%s\n------\n", (long)EXCP.pid, EXCP.identity, EXCP.status, EXCP.message);

		/* Clean used data */
		memset(&EXCP, 0, sizeof(struct _EXCP));
	}

	close(fdMainConnRequest);
	close(fdMainConnResponse);

	unlink(GTU_PRO_REQ);
	unlink(GTU_PRO_RES);
	return 0;
}