#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BUFFER_SIZE 256

void communication(int);

int main(int argc, char const *argv[])
{
    /* General variables */
    int quit = 1;

    /* Fork variables */
    int childPid;

    /* Socket variables */
    int port;
    int socketFD;
    int socketAcceptFD;
    struct sockaddr_in serverAddr;
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen;

    if (argc != 2)
    {
        printf("Usage: ./client <ip address>\n");
        exit(EXIT_FAILURE);
    }

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
    listen(socketFD, 1);

    clientAddrLen = sizeof(clientAddr);

    while(quit)
    {
        /* Socket waiting */
        socketAcceptFD = accept(socketFD, (struct sockaddr *) &clientAddr, &clientAddrLen);
        if (socketAcceptFD < 0)
        {
            perror("Error 332");
        }

        /* Create child process */
        childPid = fork();
        if (childPid < 0)
        {
            perror("Error 334");
            exit(EXIT_FAILURE);
        }

        /* This is the client process */
        if (childPid == 0)
        {
            close(socketFD); /* WHY */
            communication(socketAcceptFD);
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

void communication(int newSocket)
{
    int n;
    int quit = 1;
    char buffer[BUFFER_SIZE];

    while(quit)
    {
        bzero(buffer, BUFFER_SIZE);

        n = read(newSocket, buffer, BUFFER_SIZE - 1);
        if (n < 0) {
            perror("ERROR reading from socket");
            exit(EXIT_FAILURE);
        }

        printf("Here is the message: %s\n", buffer);

        n = write(newSocket,"I got your message", 18);
        if (n < 0) {
            perror("ERROR writing to socket");
            exit(EXIT_FAILURE);
        }
    }

}
