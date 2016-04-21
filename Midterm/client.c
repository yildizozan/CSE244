#include "protocol.h"


int main(int argc, char const *argv[])
{
	/* Buffers */
	char buffer[BUFFER_SIZE];

	/* Fifo variables */
	int fdMainConnRequest;
	int fdMainConnResponse;

	/* Open connection */
	fdMainConnRequest = open(GTU_PRO_REQ, O_WRONLY);
	fdMainConnResponse = open(GTU_PRO_RES, O_RDONLY);


	sprintf(buffer, "%lu", (unsigned long)getpid());
	write(fdMainConnRequest, buffer, BUFFER_SIZE);

	perror("Write:");

	read(fdMainConnResponse, buffer, BUFFER_SIZE);

	perror("Read:");

	printf("...received from the server: %s\n", buffer);

	close(fdMainConnRequest);
	close(fdMainConnResponse);

	return 0;
}
