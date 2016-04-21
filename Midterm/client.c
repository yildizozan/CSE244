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

	printf("%d\n", (int)getpid());
	printf("Input message to serwer: ");
	scanf("%s", buf);


	/* write buf to the FIFO */
	fdMainConnRequest = open(GTU_PRO_REQ, O_WRONLY);
	fdMainConnResponse = open(GTU_PRO_RES, O_RDONLY);

	/* Prepare protocol */
	EXCP.pid = getpid();
	EXCP.status = 0;

	write(fdMainConnRequest, &EXCP, sizeof(struct _EXCP));
	perror("Write:");

	read(fdMainConnResponse, &EXCP, sizeof(struct _EXCP));
	perror("Read:");

	close(fdMainConnRequest);
	close(fdMainConnResponse);

	if (EXCP.status == 1)
		printf("%d\n", EXCP.status);


	return 0;
}