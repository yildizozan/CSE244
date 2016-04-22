#include "protocol.h"

/* Signal */
void  signalHandlerClient(int);

/* Global variables */
static int fdSecureConnection;
static unsigned long serverChildPid;


int main(int argc, char const *argv[])
{
	/* Buffers */
	char buffer[BUFFER_SIZE];
	char protocol[BUFFER_SIZE];

	/* Fifo variables */
	int fdMainConnRequest;
	int fdMainConnResponse;

	/*
	*	 Open connection
	*/
	fdMainConnRequest = open(GTU_PRO_REQ, O_WRONLY);
	fdMainConnResponse = open(GTU_PRO_RES, O_RDONLY);

	/*************************************

	*	SIGNALS

	*************************************/

	signal(SIGHUP, signalHandlerClient);
	signal(SIGINT, signalHandlerClient);
	signal(SIGKILL, signalHandlerClient);
	signal(SIGQUIT, signalHandlerClient);
	signal(SIGUSR1, signalHandlerClient);
	signal(SIGUSR2, signalHandlerClient);

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
	else if (strcmp(buffer, "3") == 0)
	{
		printf("Server full\nExiting..\n");
		return 0;
	}
	else
	{
		printf("Not connected!\nExiting..\n");
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

	/*
	*	Child process kendi pid değerini bekliyor
	*/
	read(fdSecureConnection, buffer, BUFFER_SIZE);
	serverChildPid = atoi(buffer);
printf("---%lu\n", serverChildPid);
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

void signalHandlerClient(int sign)
{
	if ((sign == SIGHUP) || (sign == SIGINT) || (sign == SIGKILL) || (sign == SIGQUIT))
	{
		kill(serverChildPid, SIGUSR2);
		exit(EXIT_SUCCESS);
	}

	if (sign == SIGUSR2)
	{
		signal(SIGUSR1, SIG_IGN);
		
		printf("Server not online!\n");
		exit(EXIT_SUCCESS);
	}
}