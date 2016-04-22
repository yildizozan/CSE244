#include "protocol.h"

/* Signal */
void  signalHanflerSIGINT(int);

static char serverPid[10];

int main(int argc, char const *argv[])
{
	/* Buffers */
	char buffer[BUFFER_SIZE];
	char protocol[BUFFER_SIZE];

	/* Fifo variables */
	int fdMainConnRequest;
	int fdMainConnResponse;

	int fdSecureConnection;

	/*
	*	 Open connection
	*/
	fdMainConnRequest = open(GTU_PRO_REQ, O_WRONLY);
	fdMainConnResponse = open(GTU_PRO_RES, O_RDONLY);

	/*************************************

	*	SIGNALS

	*************************************/

	signal(SIGINT, signalHanflerSIGINT);

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

	if (strcmp(buffer, "1") == 0)
	{
		printf("Connected!\n");
	}
	else
	{
		printf("Not connected!\nExiting..");
		return 0;
	}

	/******************************************
	
	*	Preparing secure connection

	*******************************************/
	sprintf(protocol, "%lu", (unsigned long)getpid());
	snprintf(buffer, GTU_PRO_LEN, GTU_PRO_SEC, protocol);
printf("----%s\n", buffer);

	mkfifo(buffer, 0666);
	fdSecureConnection = open(buffer, O_RDWR);

	read(fdSecureConnection, buffer, BUFFER_SIZE);
	strcpy(serverPid, buffer);

	/* Sending argümans */
	while(1)
	{
		read(fdSecureConnection, buffer, BUFFER_SIZE);
		printf("%s\n", buffer);
	}

	close(fdSecureConnection);

	return 0;
}

/*****************************************************

	FUNCTIONS:	Signals

	SUMMERY	

	COMMENTS

*****************************************************/

void signalHanflerSIGINT(int sign)
{

	if(sign == SIGINT)

		printf("Ctrl + C yakalandi\n");
		exit(1);
}