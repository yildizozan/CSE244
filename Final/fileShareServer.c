#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <dirent.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/shm.h>
#include <netinet/in.h>

#define BUFFER_SIZE 256
#define MAX_CLIENT 100

struct clientList
{
    pid_t pid;
    char connectionTime[BUFFER_SIZE];
    unsigned int status;
};

/*
||  Global variables
*/
    int socketFD;
    int shutdownServer = 0;

        /* Shared Memory variables */
    int shmID;
    struct clientList *activeClients;

/*
||  Function Prototype
*/
void Communication(int);
void KillAllChild(void);
void listServer(void);

int clientControl(pid_t);
int clientAdd(void);
void clientDelete(int);
void clientList(void);

void writeLog(char *);

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



    /* Signal */
    struct sigaction action;
    action.sa_handler = SignalHandler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;

    sigaction(SIGINT, &action, NULL);

    /* Command line args controls */
    if (argc != 2)
    {
        printf("Usage: ./client <ip address>\n");
        exit(EXIT_FAILURE);
    }

    /*
    *   Creating Shared Memory for active clients
    */
    shmID = shmget(IPC_PRIVATE, MAX_CLIENT * sizeof(struct clientList), IPC_CREAT | 0666);
    if (shmID < 0)
    {
        perror("Error 326");
        exit(EXIT_FAILURE);
    }

    activeClients = (struct clientList *) shmat(shmID, NULL, 0);
    /* How to control activeClients */


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
            continue;
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
        else if (childPid == 0) /* This is the client process */
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

    /* Close TCP socket */
    close(socketFD);

    /* Shared Memory detach */
    shmdt((void *) activeClients);
    shmctl(shmID, IPC_RMID, NULL);

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
    int sequenceNumber;

    int quit = 1;

    char buffer[BUFFER_SIZE];

    /*
    ||  Register clients table
    */

    /* Control for exist active client */
    sequenceNumber = clientControl(getpid());
    if (-1 < sequenceNumber)
    {
        sequenceNumber = -1;

        clientDelete(sequenceNumber);   /* Client ending session */
        kill(activeClients[sequenceNumber].pid, SIGTERM);

        while(sequenceNumber == -1)     /* Client start new session */
        {
            sequenceNumber = clientAdd();
        }
    }
    else
    {
        while(sequenceNumber == -1)     /* Client start new session */
        {
            sequenceNumber = clientAdd();
        }
    }

    while(quit)
    {
        bzero(buffer, BUFFER_SIZE);

        n = read(newSocket, buffer, BUFFER_SIZE - 1);
        if (n < 1)
        {
            quit = 0;
            break;
        }
        else
        {
            printf("Here is the message: %s\n", buffer);
        }

        if (strcmp(buffer, "quit") == 0)
        {
            quit = 0;
            break;
        }
        else if (strcmp(buffer, "activeClients") == 0)
        {
            clientList();
            continue;
        }
        else if (strcmp(buffer, "listServer") == 0)
        {
            listServer();
            continue;
        }


        n = write(newSocket,"I got your message", 18);
        if (n < 0) {
            perror("ERROR writing to socket");
            exit(EXIT_FAILURE);
        }
    }

    /* Close socket for communication */
    close(newSocket);

    /* Delete myself in the clients table */
    clientDelete(sequenceNumber);

    exit(EXIT_SUCCESS);
}

/*
*   FUNCTIONS:  Shared Memory Operations
*
*   PURPOSE:    Client'leri shared memorydeki tablolara ekleme çıkarma
*               fonksiyonlarını içerir
*
*   COMMITS:
*/
int clientControl(pid_t childPid)
{
    int i;

    for (i = 0; i < MAX_CLIENT; ++i)
    {
        if (activeClients[i].pid == childPid)
        {
            return i;
        }
    }

    return -1;
}

int clientAdd(void)
{
    int i;

    for (i = 0; i < MAX_CLIENT; ++i)
    {
        if (activeClients[i].pid == 0)
        {
            activeClients[i].pid = getpid();

            /* Login write log file */
            writeLog("Login");

            return i;
        }
    }

    return -1;
}

void clientDelete(int pSequenceNumber)
{
    activeClients[pSequenceNumber].pid = 0;

    /* Logout write log file */
    writeLog("Logout");
}

void clientList(void)
{
    int i;

    for (i = 0; i < MAX_CLIENT; ++i)
    {
        printf("%d client: %ld\n", i, (long) activeClients[i].pid);
    }
}


/*
*
*
*
*/
void KillAllChild(void)
{
    int i;

    for (i = 0; i < MAX_CLIENT; ++i)
    {
        kill(activeClients[i].pid, SIGUSR1);
    }
}

void listServer(void)
{
    int n;
    DIR *directory;
    struct dirent *directoryEntry;

    char cwd[BUFFER_SIZE];
    char buffer[BUFFER_SIZE];

    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
        perror("Error 663");
        return;
    }

    strcat(cwd, "/storage");
    fprintf(stdout, "Current working dir: %s\n", cwd);

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

        snprintf(buffer, BUFFER_SIZE, "\t%s\n", directoryEntry->d_name);
        n = write(socketFD, buffer, BUFFER_SIZE);
        if (n < 0)
        {
            printf("Data gönderilemedi!\n");
        }

    }
}

/*
*   FUNCTIONS:  writeLog
*
*   PURPOSE:
*
*   COMMITS:
*/
void writeLog(char *string)
{
    FILE *fd;

    char buffer[100];

    time_t current_time;
    char* c_time_string;

    /* Obtain current time. */
    current_time = time(NULL);

    if (current_time == ((time_t)-1))
    {
        (void) fprintf(stderr, "Failure to obtain the current time.\n");
        exit(EXIT_FAILURE);
    }

    /* Convert to local time format. */
    c_time_string = ctime(&current_time);

    if (c_time_string == NULL)
    {
        (void) fprintf(stderr, "Failure to convert the current time.\n");
        exit(EXIT_FAILURE);
    }

    /* Print to stdout. ctime() has already added a terminating newline character. */
    snprintf(buffer, BUFFER_SIZE, "%ld %s %s", (long) getpid(), string, c_time_string);
    printf("%s", buffer);

    fd = fopen("Server.log", "a+");
    fprintf(fd, "%s", buffer);
    fclose(fd);
}
