/***
 *                                                                                                                           
 *      ,ad8888ba,                                           8b        d8  88  88           88888888ba,    88  888888888888  
 *     d8"'    `"8b                                           Y8,    ,8P   88  88           88      `"8b   88           ,88  
 *    d8'        `8b                                           Y8,  ,8P    88  88           88        `8b  88         ,88"   
 *    88          88  888888888  ,adPPYYba,  8b,dPPYba,         "8aa8"     88  88           88         88  88       ,88"     
 *    88          88       a8P"  ""     `Y8  88P'   `"8a         `88'      88  88           88         88  88     ,88"       
 *    Y8,        ,8P    ,d8P'    ,adPPPPP88  88       88          88       88  88           88         8P  88   ,88"         
 *     Y8a.    .a8P   ,d8"       88,    ,88  88       88          88       88  88           88      .a8P   88  88"           
 *      `"Y8888Y"'    888888888  `"8bbdP"Y8  88       88          88       88  88888888888  88888888Y"'    88  888888888888  
 *                                                                                                                           
 *                                                                                                                           
 *                                                                                                                           
 *        88  8888888888    88     ,a8888a,             ,d8            ,d8       ,a8888a,      ad88888ba    ad888888b,       
 *      ,d88  88          ,d88   ,8P"'  `"Y8,         ,d888          ,d888     ,8P"'  `"Y8,   d8"     "8b  d8"     "88       
 *    888888  88  ____  888888  ,8P        Y8,      ,d8" 88        ,d8" 88    ,8P        Y8,  Y8a     a8P          a8P       
 *        88  88a8PPPP8b,   88  88          88    ,d8"   88      ,d8"   88    88          88   "Y8aaa8P"        ,d8P"        
 *        88  PP"     `8b   88  88          88  ,d8"     88    ,d8"     88    88          88   ,d8"""8b,      a8P"           
 *        88           d8   88  `8b        d8'  8888888888888  8888888888888  `8b        d8'  d8"     "8b   a8P'             
 *        88  Y8a     a8P   88   `8ba,  ,ad8'            88             88     `8ba,  ,ad8'   Y8a     a8P  d8"               
 *        88   "Y88888P"    88     "Y8888P"              88             88       "Y8888P"      "Y88888P"   88888888888       
 *                                                                                                                           
 *                                                                                                                           
 */

#include "protocol.h"

void addClient(struct _EXCP client, const int i);
void deleteClient(pid_t pidClient);

/* Signal */
<<<<<<< HEAD
void  signalHandlerServer(int);
void signalHandlerChild(int sign);
=======
void  signalHandler(int);
>>>>>>> origin/develop

/* Other functions */
void openingStyle1(void);
void openingStyle2(void);



/* Global variables */
static struct _EXCP activeClientTable[255];
static int maxClients;
static int activeClients = 0;




int main(int argc, char const *argv[])
{
	/* Protocol */
    struct _EXCP EXCP, client;

	/* Counters */
	int i;
	int status;

	/* Control variables */
	int disconnect;
	int emptySlot;

	/* Buffers */
	char buffer[BUFFER_SIZE];

	/* Fifo variables */
	int fdMainConnection;
	char nameNewSecureConnection[BUFFER_SIZE];
	int fdNewSecureConnection;

	/* Pipe variables */
	int pipeDescriptionForServerClient[2];

	/* Fork variables */
	pid_t childPid;

	/* Argumans vaariables */
/*	float resulution = atof(argv[1]);*/
	maxClients = atoi(argv[2]);

	/*************************************

	*   SIGNALS

	*************************************/

<<<<<<< HEAD
	signal(SIGHUP, signalHandlerServer);
	signal(SIGINT, signalHandlerServer);
	signal(SIGKILL, signalHandlerServer);
	signal(SIGQUIT, signalHandlerServer);
	signal(SIGUSR1, signalHandlerServer);
	signal(SIGUSR2, SIG_IGN);
=======
	signal(SIGHUP, signalHandler);
	signal(SIGINT, signalHandler);
	signal(SIGKILL, signalHandler);
	signal(SIGQUIT, signalHandler);
>>>>>>> origin/develop


	/*
	*	Prepare server
	*/

	for ( i = 0; i < maxClients; ++i)
		activeClientTable[i].pidClient = 0;

<<<<<<< HEAD
=======
	*************************************/
	if ((childPid =fork()) == 0) /* Child */
	{
		openingStyle2();
		exit(EXIT_SUCCESS);
	}
	else /* Main */
	{
		addChild(childPid);

		/*************************************
>>>>>>> origin/develop


	mkfifo(GTU_PRO_NAM, 0666);
	fdMainConnection = open(GTU_PRO_NAM, O_RDWR);

<<<<<<< HEAD
	while(1)
	{
		/* Reset variables */
		disconnect = 0;
=======
		/*
		*	Active client table reset
		*/
		for (i = 0; i < maxClients; ++i)
			strcpy(activeClientTable[i].pid, "");
>>>>>>> origin/develop

		/*
		*	Waiting client
		*/
		read(fdMainConnection, &EXCP, sizeof(struct _EXCP));

		{
			client.pidClient = EXCP.pidClient;
			client.status = EXCP.status;
			strcpy(client.identity, EXCP.identity);
		}

		/*
		*	Gelen talebi inceliyoruz
		*/
		if (EXCP.status == -1)
		{
			printf("Error %d\n", EXCP.status);
		}
		else if (EXCP.status == 1)
		{
			/* Register için boş yer arıyoruz */
			for (i = 0; i < maxClients; ++i)
				if (activeClientTable[i].pidClient == 0)
					emptySlot = i;


			/*
			*	Maximum kullanici sayisina ulasildiysa eger
			*/
			if (activeClients == maxClients)
			{
				EXCP.status = -1;
				write(fdMainConnection, &EXCP, sizeof(struct _EXCP));
				continue;
			}

			/*
			*	Prepare protocol
			*/
			snprintf(buffer, GTU_PRO_LEN, GTU_PRO_SEC, (unsigned long)EXCP.pidClient);
			strcpy(EXCP.identity, buffer);
			strcpy(client.identity, EXCP.identity);			
			EXCP.status = 2;
			write(fdMainConnection, &EXCP, sizeof(struct _EXCP));

			mkfifo(EXCP.identity, 0666);
			pipe(pipeDescriptionForServerClient);
			if ((childPid = fork()) < 0)
			{
				perror("Error 156");
			}
			else
			{
				/*
				*	Child process
				*/
				if (childPid == 0)
				{
					int a = 0; /* DElete */

					/*************************************

					*   SIGNALS

					*************************************/

<<<<<<< HEAD
					signal(SIGHUP, signalHandlerChild);
					signal(SIGINT, signalHandlerChild);
					signal(SIGKILL, signalHandlerChild);
					signal(SIGQUIT, signalHandlerChild);
					signal(SIGUSR1, signalHandlerChild);
					signal(SIGUSR2, SIG_IGN);
=======
						write(fdNewSecureConnection, buffer, BUFFER_SIZE);
						read(fdNewSecureConnection, buffer, BUFFER_SIZE);
						if (strcmp(buffer, "7") == 0)
							break;

						sleep(1);
					}
>>>>>>> origin/develop

					/* Child kendi bilgilerini doldurup parenta yolluyor */
					EXCP.pidChild = getpid();

					close(pipeDescriptionForServerClient[0]);
					write(pipeDescriptionForServerClient[1], &EXCP, sizeof(struct _EXCP));
					close(pipeDescriptionForServerClient[1]);

					close(pipeDescriptionForServerClient[1]);
					write(pipeDescriptionForServerClient[0], &EXCP, sizeof(struct _EXCP));
					close(pipeDescriptionForServerClient[0]);


					fdNewSecureConnection = open(EXCP.identity, O_RDWR);

					while(1)
					{
						a++;
						EXCP.status = a;
						write(fdNewSecureConnection, &EXCP, sizeof(struct _EXCP));
						sleep(1);
					}

					close(fdNewSecureConnection);
				}
				/*
				*	Parent
				*/
				else
				{	
					/*
					*	Child kendi pid sini yollayacak
					*	Parent ise bunu kullanarak register edecek
					*/
					close(pipeDescriptionForServerClient[1]);
					read(pipeDescriptionForServerClient[0], &EXCP, sizeof(struct _EXCP));
					close(pipeDescriptionForServerClient[0]);


<<<<<<< HEAD
					/* 
					*	Child kimligi eklendi client yapısına
					*/
					client.pidChild = EXCP.pidChild;

					/*
					*	Tamamlanmış bilgiler gidiyor artık
					*/
					close(pipeDescriptionForServerClient[0]);
					read(pipeDescriptionForServerClient[1], &client, sizeof(struct _EXCP));
					close(pipeDescriptionForServerClient[1]);

					/*
					*	Register ediyoruz artık sorun yok
					*/
					addClient(client, emptySlot);

printf("client.pidClient: %d\n", (int)client.pidClient);
printf("client.pidChild: %d\n", (int)client.pidChild);
printf("client.identity: %s\n", client.identity);
printf("client.data: %s\n", client.data);
printf("client.status: %d\n", client.status);
printf("\n");


				}
			}

		}
=======
void signalHandler(int sign)
{
	if (sign == SIGHUP)
		printf("Catch HUP\n");
	if (sign == SIGINT)
		printf("Catch Ctrl + C\n");
	if (sign == SIGKILL)
		printf("Durdurulamaz\n");
	if (sign == SIGQUIT)
		printf("Belki durdurulur\n");
}


/*****************************************************
>>>>>>> origin/develop

		/*
		*	Clean data
		*/
	}

	close(fdMainConnection);

    return 0;
}

void addClient(struct _EXCP client, const int i)
{

}

void deleteClient(pid_t pidClient)
{
	int i;
	for (i = 0; i < maxClients; ++i)
	{
		if (activeClientTable[i].pidClient == pidClient)
		{
			activeClientTable[i].pidClient = 0;
		}
	}
}