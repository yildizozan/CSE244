#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define BUFFER_SIZE 256

int main(int argc, char const *argv[])
{
    /* General variables */
    int quit = 1;
    int n;

	/* Buffer */
    char buffer[BUFFER_SIZE];

    /* Socket variables */
    int port = 9303;
    int socketFD;
    struct sockaddr_in clientAddr;
    struct hostent *server;

    if (argc != 2)
    {
        printf("Usage: ./client <ip address>\n");
        exit(EXIT_FAILURE);
    }

	/* Create a socket */
    socketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFD < 0)
    {
        perror("Error 1223");
        exit(EXIT_FAILURE);
    }

    server = gethostbyname(argv[1]);
    if (server == NULL)
    {
		fprintf(stderr, "Server name not true!\n");
        exit(EXIT_FAILURE);
    }

    bzero((char *) &clientAddr, sizeof(clientAddr));
    clientAddr.sin_family = AF_INET;
    bcopy(
    	(char *)server->h_addr,
    	(char *) &clientAddr.sin_addr.s_addr,
    	server->h_length
    );
    clientAddr.sin_port = htons(port);

    /* Connect to the server */
    if (connect(socketFD, (struct sockaddr *) &clientAddr, sizeof(clientAddr)) < 0)
    {
    	perror("Error 1224");
    	exit(EXIT_FAILURE);
    }

    /* Welcome */
    printf("Welcome File Share Server v1 !\n");

    while(quit)
    {
	    /*
	    *	Send Message!
	    */
	    printf("-> ");
	    bzero(buffer, BUFFER_SIZE);
	    fgets(buffer, BUFFER_SIZE, stdin);

	    /* Send Message */
	    n = write(socketFD, buffer, strlen(buffer));
	    if (n < 0)
	    {
	    	perror("Error 1335");
	    	exit(EXIT_FAILURE);
	    }

	    /* Read server message */
	    bzero(buffer, BUFFER_SIZE);

	    n = read(socketFD, buffer, BUFFER_SIZE);
	    if (n < 0)
	    {
	    	perror("Error 1335");
	    	exit(EXIT_FAILURE);
	    }

	    printf("%s\n", buffer);
    }


    return 0;
}
