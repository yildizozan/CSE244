#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>

#define BUFFER_SIZE 256

struct clientList
{
    long pid;
    char connectionTime[BUFFER_SIZE];
    unsigned int status;
};

/*
*   Global variables 
*/
int socketFD;
int shutdownServer = 0;
struct clientList activeClients[100];

/*
*   Function Prototype
*/
void Communication(int);
void KillAllChild(void);
void listServer(void);

void SignalHandler(int sign)
{
    if (sign == SIGCHLD)
    {
        wait3(NULL, WNOHANG , NULL);
    }
    if (sign == SIGINT)
    {
        printf("received SIGINT\n");
        shutdownServer = 1;
    }    
}

int main(int argc, char const *argv[])
{
    /* General variables */

    /* Fork variables */
    int childPid;

    /* Pipe description */
    int pipeDescription[2];

    /* Socket variables */
    int port;
    int socketAcceptFD;
    struct sockaddr_in serverAddr;
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen;

    /*
    *   Signal
    */
    struct sigaction action;
    action.sa_handler = SignalHandler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;

    if (argc != 2)
    {
        printf("Usage: ./client <ip address>\n");
        exit(EXIT_FAILURE);
    }

    /*sigaction(SIGCHLD, &SignalHandler, NULL);*/
    sigaction(SIGINT, &action, NULL);

    /* Creating socket */
    socketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFD < 0)
    {
        perror("Error 213");
        exit(EXIT_FAILURE);
    }

    /* Initialize socket */
    bzero((char *) &serverAddr, sizeof(serverAddr));

    port = atoi(argv[1]);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    /* Socket Bind */
    if (bind(socketFD, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0)
    {
        perror("Error 214");
        exit(EXIT_FAILURE);
    }

    /* Starting to liston for the clientsi now! */
    listen(socketFD, 5);

    clientAddrLen = sizeof(clientAddr);

    while(shutdownServer == 0)
    {
        /* Socket waiting */
        socketAcceptFD = accept(socketFD, (struct sockaddr *) &clientAddr, &clientAddrLen);
        if (socketAcceptFD < 0)
        {
            perror("Error 332");
        }

        if (pipe(pipeDescription) < 0)
        {
            perror("Error 333");
        }

        /* Create child process */
        childPid = fork();
        if (childPid < 0)
        {
            perror("Error 334");
        }

        /* This is the client process */
        if (childPid == 0)
        {
            close(socketFD);
            Communication(socketAcceptFD);
            exit(EXIT_SUCCESS);
        }
        else
        {
            close(socketAcceptFD);
        }
    }

    close(socketFD);

    return 0;
}

/*
*   FUNCTIN: SignalHandler
*
*   PURPOSE: Catch signals
*/


void Communication(int newSocket)
{
    int n;

    int quit = 1;
    char quitVariable[] = "quit";

    char buffer[BUFFER_SIZE];

    while(quit)
    {
        bzero(buffer, BUFFER_SIZE);

        n = read(newSocket, buffer, BUFFER_SIZE - 1);
        if (n < 0) {
            perror("ERROR reading from socket");
            exit(EXIT_FAILURE);
        }

        if (strcmp(buffer, quitVariable) == 0)
        {
            quit = 0;
        }

        printf("Here is the message: %s\n", buffer);

        n = write(newSocket,"I got your message", 18);
        if (n < 0) {
            perror("ERROR writing to socket");
            exit(EXIT_FAILURE);
        }
    }

    close(newSocket);

}

void KillAllChild(void)
{
    int i;

    for (i = 0; i < 100; ++i)
    {
        kill(activeClients[i].pid, SIGKILL);
    }
}

void listServer(void)
{
    char cwd[BUFFER_SIZE];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
        fprintf(stdout, "Current working dir: %s\n", cwd);
    else
        perror("getcwd() error");
}