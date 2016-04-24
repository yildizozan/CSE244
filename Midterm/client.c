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

    /* Fifo variables */
    int fdMainConnection;
    int fdNewSecureConnection;

    /* Argumans */
    struct _CALP CALP;
    strcpy(CALP.fi, argv[1]);
    CALP.operand = argv[4];
    strcpy(CALP.fi, argv[1]);
    CALP.timeInterval = atof(argv[3]);


    /*************************************

    *   SIGNALS

    *************************************/
    signalClient.sa_handler = signalHandlerClient;
    sigemptyset (&signalClient.sa_mask);
    signalClient.sa_flags = SA_RESETHAND;

    sigaction(SIGINT, &signalClient, NULL);
    sigaction(SIGUSR2, &signalClient, NULL);


    /*
    *   Send request
    */ 
    EXCP.pidClient = getpid();
    EXCP.status = 1;
    snprintf(EXCP.identity, GTU_PRO_LEN, GTU_PRO_SEC, (long)getpid());

    /*
    *   Open main connection
    */
    fdMainConnection = open(GTU_PRO_NAM, O_RDWR);

    /*
    *   Send request EXCP packet
    */
    write(fdMainConnection, &EXCP, sizeof(struct _EXCP));
        perror("Request");

    /*
    *   Waiting response
    */
    read(fdMainConnection, &EXCP, sizeof(struct _EXCP));
        perror("Response");

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

    write(fdNewSecureConnection, &CALP, sizeof(struct _CALP));


    while(1)
    {
        read(fdNewSecureConnection, &EXCP, sizeof(struct _EXCP));

        printf("client.pidClient: %d\n", (int)EXCP.pidClient);
        printf("client.pidChild: %d\n", (int)EXCP.pidChild);
        printf("client.identity: %s\n", EXCP.identity);
        printf("client.data: %s\n", EXCP.data);
        printf("client.time: %s\n", EXCP.time);
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
    if (sign == SIGUSR2)
    {
        printf("Server shut down!\n");
        exit(EXIT_SUCCESS);
    }

    if (sign == SIGINT || sign == SIGKILL || sign == SIGTERM || sign == SIGQUIT)
    {
        kill(EXCP.pidChild, SIGUSR2);
        exit(EXIT_SUCCESS);
    }
}