#include "protocol.h"


int main(int argc, char const *argv[])
{
	/* Buffers */
	char buffer[BUFFER_SIZE];

	/* Fifo variables */
	int fdMainConnRequest;
	int fdMainConnResponse;

	/*
	*	 Open connection
	*/
	fdMainConnRequest = open(GTU_PRO_REQ, O_WRONLY);
	fdMainConnResponse = open(GTU_PRO_RES, O_RDONLY);

	/*
	*	Send request
	*/
	sprintf(buffer, "%lu", (unsigned long)getpid());
	write(fdMainConnRequest, buffer, BUFFER_SIZE);
	perror("Request sending");

	/*
	*	Waiting response
	*/
	read(fdMainConnResponse, buffer, BUFFER_SIZE);
	perror("Response sending");

	/*
	*	Close main connections
	*/
	close(fdMainConnRequest);
	close(fdMainConnResponse);

	if (strcmp(buffer, "2") == 0)
	{
		printf("Connected!\n");
	}
	else
	{
		printf("Not connected!\nExiting..");
		return 0;
	}


	while(1)
	{
		
	}


	return 0;
}
