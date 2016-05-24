#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>

#define BUFFER_SIZE 256

void listLocal(void);
void help(void);

void SignalHandler(int);

int main(int argc, char const *argv[])
{
    /* General variables */
    int quit = 1;
    int n;

	/* Buffer */
    char buffer[BUFFER_SIZE];

    /* Socket variables */
    int port = atoi(argv[2]);
    int socketFD;
    struct sockaddr_in serverAddr;
    struct hostent *server;

    if (argc != 3)
    {
        printf("Usage: ./client <ip address>\n");
        exit(EXIT_FAILURE);
    }

    /*
    *   Signal
    */

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

    bzero((char *) &serverAddr, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    bcopy(
    	(char *)server->h_addr,
    	(char *) &serverAddr.sin_addr.s_addr,
    	server->h_length
    );
    serverAddr.sin_port = htons(port);

    /* Connect to the server */
    if (connect(socketFD, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0)
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

        buffer[strlen(buffer)-1] = '\0';

        if (strcmp(buffer, "help") == 0)
        {
            printf("Help yazdın!\n");
            help();
            continue;
        }
        if (strcmp(buffer, "listLocal") == 0)
        {
            printf("listLocal yazdın!\n");
            listLocal();
            continue;
        }
        if (strcmp(buffer, "send") == 0)
        {
            /* Send Message */
            n = write(socketFD, buffer, strlen(buffer));
            if (n < 0)
            {
                perror("Error 1335");
                exit(EXIT_FAILURE);
            }

            /* Buffer cleaning */
            bzero(buffer, BUFFER_SIZE);

            /* Read server message */
            n = read(socketFD, buffer, BUFFER_SIZE);
            if (n < 0)
            {
                perror("Error 1335");
                exit(EXIT_FAILURE);
            }
        }
    }


    return 0;
}

void SignalHandler(int sign)
{
    if (sign == SIGINT)
    {
        printf("SIGINT signal catched!\n");
    }
    else if (sign == SIGTSTP)
    {
        printf("SIGTSTP signal catched!\n");
    }
}

void listLocal(void)
{
    char cwd[BUFFER_SIZE];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
        fprintf(stdout, "Current working dir: %s\n", cwd);
    else
        perror("getcwd() error");
}

void help(void)
{

}
