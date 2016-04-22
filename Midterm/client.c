#include "protocol.h"

/* Signal */
static void signalHandlerClient(int);

/* Protocol */
static struct _EXCP EXCP;

int main(int argc, char const *argv[])
{
    /* Signal */
    struct sigaction signalClient;

    /* Buffers */
    char buffer[BUFFER_SIZE];

    /* Fifo variables */
    int fdMainConnection;
    int fdNewSecureConnection;

    /*************************************

    *   SIGNALS

    *************************************/
    signalClient.sa_handler = signalHandlerClient;
    sigemptyset (&signalClient.sa_mask);
    signalClient.sa_flags = SA_RESETHAND;

    sigaction(SIGINT, &signalClient, 0);


    /*
    *   Send request
    */ 
    EXCP.pidClient = getpid();
    EXCP.status = 1;
    snprintf(EXCP.identity, GTU_PRO_LEN, GTU_PRO_SEC, (long)getpid());
    printf("--Control identity %s\n", EXCP.identity);

    /*
    *   Open main connection
    */
    fdMainConnection = open(GTU_PRO_NAM, O_RDWR);

    /*
    *   Send request EXCP packet
    */
    write(fdMainConnection, &EXCP, sizeof(struct _EXCP));
        perror("Request sending");

    /*
    *   Waiting response
    */
    read(fdMainConnection, &EXCP, sizeof(struct _EXCP));
        perror("Response sending");

    /*
    *   Close main connections
    */
    close(fdMainConnection);


    if (EXCP.status == -1)
    {
        printf("Server full!\nExiting..");
        exit(EXIT_FAILURE);
    }
    if (EXCP.status != 2)
    {
        printf("Not new connection\nExiting..");
        exit(EXIT_FAILURE);
    }

    /******************************************
    
    *   Preparing secure connection

    *******************************************/
    printf("Connected!\n");

    fdNewSecureConnection = open(EXCP.identity, O_RDWR);

    while(1)
    {
        read(fdNewSecureConnection, &EXCP, sizeof(struct _EXCP));

        printf("client.pidClient: %d\n", (int)EXCP.pidClient);
        printf("client.pidChild: %d\n", (int)EXCP.pidChild);
        printf("client.identity: %s\n", EXCP.identity);
        printf("client.data: %s\n", EXCP.data);
        printf("client.status: %d\n", EXCP.status);
        printf("\n");

    }

    close(fdNewSecureConnection);
    

    return 0;
}

/*****************************************************

    FUNCTIONS:  Signals

    SUMMERY 

    COMMENTS

*****************************************************/

void signalHandlerClient(int sign)
{
    if (sign == SIGINT)
    {
        printf("Catch signal!\n");
        kill(EXCP.pidChild, SIGUSR2);
        exit(EXIT_SUCCESS);
    }

    if (sign == SIGUSR2)
    {
        printf("Server not online!\n");
        exit(EXIT_SUCCESS);
    }
}