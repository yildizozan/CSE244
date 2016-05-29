#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <netdb.h>

#define BUFFER_SIZE 256

/* Globals */
int socketFD;

void listLocal(char []);
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
	    *	Enter command
	    */
	    printf("-> ");
	    bzero(buffer, BUFFER_SIZE);
	    fgets(buffer, BUFFER_SIZE, stdin);

        buffer[strlen(buffer)-1] = '\0';

        if (strcmp(buffer, "help") == 0)
        {
            help();

            continue;
        }
        else if (strcmp(buffer, "activeClients") == 0)
        {
            n = write(socketFD, buffer, strlen(buffer));
            if (n < 0)
            {
                perror("Error 1335");
                exit(EXIT_FAILURE);
            }

            continue;
        }
        else if (strcmp(buffer, "listLocal") == 0)
        {
            getcwd(buffer, sizeof(buffer));
            fprintf(stdout, "Current working dir: %s\n", buffer);

            listLocal(buffer);

            continue;
        }
        else if (strcmp(buffer, "listServer") == 0)
        {
            printf("listServer yazdın!\n");

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

            continue;
        }
        else if (strcmp(buffer, "send") == 0)
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

            continue;
        }
        else
        {
            printf("Geçersiz komut..\n");

            continue;
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

/*
*   FUNCTIONS: Sending, Reveiving
*
*
*/
int Sending(char buffer[BUFFER_SIZE])
{
    int n;

    n = write(socketFD, buffer, strlen(buffer));
    if (n < 0)
    {
        perror("Error 1335");
        return 0;
    }

    return 1;
}

int Receiving(void)
{
    int n;
    char buffer[BUFFER_SIZE];

    /* Buffer cleaning */
    bzero(buffer, BUFFER_SIZE);

    /* Read server message */
    n = read(socketFD, buffer, BUFFER_SIZE);
    if (n < 0)
    {
        perror("Error 1336");
        return 0;
    }

    return 1;
}

/*
*   FUNCTION: listLocal
*/
void listLocal(char cwd[BUFFER_SIZE])
{
    DIR *directory;
    struct dirent *directoryEntry;

    directory = opendir(cwd);
    if (directory == NULL)
    {
        perror("Error 664");
        return;
    }

    while ((directoryEntry = readdir(directory)) != NULL)
    {
        if (strcmp(directoryEntry->d_name, ".") == 0 || strcmp(directoryEntry->d_name, "..") == 0)
            continue;

        else if (directoryEntry->d_type == DT_DIR)
        {
            strcat(cwd, "/");
            strcat(cwd, directoryEntry->d_name);
            printf("->zsc\n");
            printf("%s\n", cwd);
            listLocal(cwd);

        }
        else if (directoryEntry->d_type == DT_REG)
        {
            printf("\t%s\n", directoryEntry->d_name);
        }
    }
}
void help(void)
{
    printf("Modules: \n");
    printf("\tlistServer\n");
    printf("\tlistLocal\n");
    printf("\tsend <id> <file of full path>\n");
}
