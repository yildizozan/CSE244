#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>


#define BUFFER_SIZE 4096

int main(int argc, char *argv[])
{
	/*
	if (argc != 3)
	{
	printf("Wrong parameters!\n");
	printf("Usage: ./server <(int)resulution> <(int)maxClient>\n");
	exit(EXIT_FAILURE);
	}
	*/

	/* Argumans vaariables */
	float resulution = atof(argv[1]);
	int maxClient = atoi(argv[2]);

	/* Fifo status */
	int fifoFileDescripton;
	char fifoBuffer[BUFFER_SIZE];

	printf("%f - %d\n", resulution, maxClient);

	/*
	We'll try to create fifo file for connection
	if there is a fifo connection file, exiting for security
	*/
	if (mkfifo("Connection", 0666) < 0 && (errno != EEXIST))
	{
		printf("Cannot create 'Connection' fifo file..\n");
		printf("We'll exiting..\n");
		exit(EXIT_FAILURE);
	}

	if ((fifoFileDescripton = open("Connection", O_RDWR)) < 0)
	{
		printf("Cannot open fifo file, please try again\n");
		printf("We'll exiting..\n");
		exit(EXIT_FAILURE);
	}
	else
		printf("Server working and waiting client(s)..\n");

	/*
	Waiting clients
	*/
	while (1)
	{	/* If success */
		read(fifoFileDescripton, fifoBuffer, sizeof(fifoBuffer));

		/*
		Close server commant
		*/
		if (strcmp(fifoBuffer, "exit") == 0)
		{
			printf("U r admin!\nShut down server..\n");
			exit(EXIT_SUCCESS);
		}

		printf("Client pid: %s connecting..\n", fifoBuffer);

		strcpy(fifoBuffer, "OK!");
		write(fifoFileDescripton, fifoBuffer, sizeof(fifoBuffer));



		memset(fifoBuffer, 0, BUFFER_SIZE);

	} /* end while */

	  /*
	  Close and delete fifo files
	  */
	close(fifoFileDescripton);
	unlink("Connection");

	return 1;
}
/*
cd '/root/Desktop/Midterm/'

*/