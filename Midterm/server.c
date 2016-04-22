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


/* Other functions */
void openingStyle1(void);
void openingStyle2(void);


/* Global variables */
static struct _EXCP client;
static struct _EXCP activeClientTable[255];
static int maxClients;
static int activeClients = 0;


/* Signal */
static void signalHandlerServer(int);
static void signalHandlerChild(int);


int main(int argc, char const *argv[])
{
	/* Signal */
	struct sigaction signalMain;

	/* Protocol */
    struct _EXCP EXCP, client;

	/* Counters */
	int i;
	int status;

	/* Control variables */
	int emptySlot;

	/* Buffers */
	char buffer[BUFFER_SIZE];

	/* Fifo variables */
	int fdMainConnection;
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
	signalMain.sa_handler = signalHandlerServer;
	sigemptyset (&signalMain.sa_mask);
	signalMain.sa_flags = SA_RESTART;

	sigaction(SIGINT, &signalMain, NULL);
	sigaction(SIGUSR1, &signalMain, &signalOld);


	/*********************************

	*	Prepare server for open

	*********************************/

	for ( i = 0; i < maxClients; ++i)
		activeClientTable[i].pidClient = 0;



	mkfifo(GTU_PRO_NAM, 0666);
	fdMainConnection = open(GTU_PRO_NAM, O_RDWR);

	/*********************************

	*	Opening

	*********************************/
	openingStyle2();

	while(1)
	{

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
					struct sigaction signalChild;
					signalChild.sa_handler = signalHandlerChild;
					sigemptyset (&signalChild.sa_mask);
					signalChild.sa_flags = SA_RESETHAND;

				    sigaction(SIGINT, &signalChild, 0);
				    sigaction(SIGUSR1, &signalChild, 0);
				    sigaction(SIGUSR2, &signalChild, 0);


					/*
					*	Child send child pid to main process
					*/
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
					printf("client.pidClient: %d\n", (int)client.pidClient);
					printf("client.pidChild: %d\n", (int)client.pidChild);
					printf("client.identity: %s\n", client.identity);
					printf("client.data: %s\n", client.data);
					printf("client.status: %d\n", client.status);
					printf("\n");
			
					addClient(client, emptySlot);

				}

			}

		}

		/* Data reset */
		memset(&client, 0, sizeof(struct _EXCP));

	}

	close(fdMainConnection);

    return 0;
}

void addClient(struct _EXCP newClient, const int i)
{
	activeClientTable[i].pidClient = newClient.pidClient;
	activeClientTable[i].pidChild = newClient.pidChild;
	strcpy(activeClientTable[i].identity, newClient.identity);
	activeClientTable[i].status = newClient.status;
}


void deleteClient(pid_t childPid)
{
	int i;

	for (i = 0; i < maxClients; ++i)
	{
		if (activeClientTable[i].pidChild == childPid)
		{
			activeClientTable[i].pidClient = 0;
			activeClientTable[i].pidChild = 0;
			strcpy(activeClientTable[i].identity, "");
			activeClientTable[i].status = 0;
			break;
		}
	}
}

/*********************************************************

	SECTION: 	SIGNALS

	FUNCTIONS:	Main and child prcess signal handlers

	PURPOSE:	

	COMMENTS:	



************************************************************/
static void signalHandlerServer(int sign)
{
	if (sign == SIGUSR1)
	{
		pid_t childPid = waitpid(-1, NULL, 0);
		deleteClient(childPid);


		kill(childPid, SIGQUIT);
		printf("%d died\n", (int)childPid);


	}

	if (sign == SIGINT)
	{

	}
}

static void signalHandlerChild(int sign)
{

	if (sign == SIGUSR1)
	{
		printf("Catch SIGUSR1\n");

		for (int i = 0; i < maxClients; ++i)
		{
			if (activeClientTable[i].pidChild == getpid())
			{
				kill(activeClientTable[i].pidClient, SIGUSR2);
				exit(EXIT_SUCCESS);
			}
		}
	}

	if (sign == SIGUSR2)
	{
		kill(getppid(), SIGUSR1);
		exit(EXIT_SUCCESS);
	}

	if (sign == SIGINT)
	{
	}
}

/*****************************************************

	FUNCTIONS:	Opening style

	SUMMERY	

	COMMENTS

*****************************************************/

void openingStyle1(void)
{
	printf(" _______  _______  ______    __   __  _______  ______      __   __  ____\n");
	printf("|       ||       ||    _ |  |  | |  ||       ||    _ |    |  | |  ||    |\n");
usleep(500000);
	printf("|  _____||    ___||   | ||  |  |_|  ||    ___||   | ||    |  |_|  | |   |\n");
usleep(500000);
	printf("| |_____ |   |___ |   |_||_ |       ||   |___ |   |_||_   |       | |   |\n");
usleep(500000);
	printf("|_____  ||    ___||    __  ||       ||    ___||    __  |  |       | |   |\n");
usleep(500000);
	printf(" _____| ||   |___ |   |  | | |     | |   |___ |   |  | |   |     |  |   |\n");
usleep(500000);
	printf("|_______||_______||___|  |_|  |___|  |_______||___|  |_|    |___|   |___|\n");
	printf("\n");
usleep(500000);
	printf("                                                   Created by Ozan Yıldız\n");
	printf("SERVER ON!..\n");
	printf("\n");
}

void openingStyle2(void)
{
	printf("\n");
	printf(".d8888. d88888b d8888b. db    db d88888b d8888b.      db    db  db\n");
usleep(200000);
	printf("88'  YP 88'     88  `8D 88    88 88'     88  `8D      88    88 o88\n");
usleep(200000);
	printf("`8bo.   88ooooo 88oobY' Y8    8P 88ooooo 88oobY'      Y8    8P  88\n");
usleep(200000);
	printf("  `Y8b. 88~~~~~ 88`8b   `8b  d8' 88~~~~~ 88`8b        `8b  d8'  88\n");
usleep(200000);
	printf("db   8D 88.     88 `88.  `8bd8'  88.     88 `88.       `8bd8'   88\n");
usleep(200000);
	printf("`8888Y' Y88888P 88   YD    YP    Y88888P 88   YD         YP     VP\n");
	printf("\n");
usleep(200000);
	printf("                                            Created by Ozan Yıldız\n");
	printf("SERVER ON!..\n");
	printf("\n");
}

