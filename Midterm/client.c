#include "protocol.h"

/* Signal */
void  signalHandlerClient(int);


int main(int argc, char const *argv[])
{
    /* Protocol */
    struct _EXCP EXCP;

    /* Buffers */
    char buffer[BUFFER_SIZE];

    /* Fifo variables */
    int fdMainConnection;
    int fdNewSecureConnection;

    /*************************************

    *   SIGNALS

    *************************************/

    signal(SIGHUP, signalHandlerClient);
    signal(SIGINT, signalHandlerClient);
    signal(SIGKILL, signalHandlerClient);
    signal(SIGQUIT, signalHandlerClient);
    signal(SIGUSR1, SIG_IGN);
    signal(SIGUSR2, signalHandlerClient);


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
    if ((sign == SIGHUP) || (sign == SIGINT) || (sign == SIGKILL) || (sign == SIGQUIT))
    {
        exit(EXIT_SUCCESS);
    }

    if (sign == SIGUSR2)
    {
        signal(SIGUSR1, SIG_IGN);
        
        printf("Server not online!\n");
        exit(EXIT_SUCCESS);
    }
}#include "protocol.h"

/* Signal */
void  signalHandlerClient(int);


int main(int argc, char const *argv[])
{
    /* Protocol */
    struct _EXCP EXCP;

    /* Buffers */
    char buffer[BUFFER_SIZE];

    /* Fifo variables */
    int fdMainConnection;
    int fdNewSecureConnection;

    /*************************************

    *   SIGNALS

    *************************************/

    signal(SIGHUP, signalHandlerClient);
    signal(SIGINT, signalHandlerClient);
    signal(SIGKILL, signalHandlerClient);
    signal(SIGQUIT, signalHandlerClient);
    signal(SIGUSR1, SIG_IGN);
    signal(SIGUSR2, signalHandlerClient);


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
    if ((sign == SIGHUP) || (sign == SIGINT) || (sign == SIGKILL) || (sign == SIGQUIT))
    {
        exit(EXIT_SUCCESS);
    }

    if (sign == SIGUSR2)
    {
        signal(SIGUSR1, SIG_IGN);
        
        printf("Server not online!\n");
        exit(EXIT_SUCCESS);
    }
}