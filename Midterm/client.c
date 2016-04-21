#include "protocol.h"

int main(int argc, char const *argv[])
{
	int fdMainConnRequest;

	int fdMainConnResponse;

	char str[BUFFER_SIZE];
	printf("Input message to serwer: ");
	scanf("%s", str);


	/* write str to the FIFO */
	fdMainConnRequest = open(GTU_PRO_REQ, O_WRONLY);
	fdMainConnResponse = open(GTU_PRO_RES, O_RDONLY);
	write(fdMainConnRequest, str, BUFFER_SIZE);

	perror("Write:");

	read(fdMainConnResponse, str, BUFFER_SIZE);

	perror("Read:");

	printf("...received from the server: %s\n", str);
	close(fdMainConnRequest);
	close(fdMainConnResponse);

	/* remove the FIFO */

	return 0;
}