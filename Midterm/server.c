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

void addClient(const char clientName[BUFFER_SIZE], const unsigned int i);
void deleteClient(const char clientName[BUFFER_SIZE]);

void addChild(const pid_t childPid);
void deleteChild(const pid_t childPid);

/* Signal */
void  signalHandler(int);

/* Other functions */
void openingStyle1(void);
void openingStyle2(void);



/* Global variables */
static pid_t childPrecessesTable[255];
static struct _EXCP activeClientTable[255];
static int maxClients;



int main(int argc, char const *argv[])
{
	/* Counters */
	int i;

	/* Control variables */
	int activeClients = 0;
	int disconnect;
	int emptySlot;

	/* Buffers */
	char buffer[BUFFER_SIZE];

	/* Pipe variables */
	int pipeDescriptionForServerClient[2];

	/* Fifo variables */
	int fdMainConnRequest;
	int fdMainConnResponse;
	char nameNewSecureConnection[BUFFER_SIZE];
	int fdNewSecureConnection;


	/* Fork variables */
	pid_t childPid;

	/* Argumans vaariables */
/*	float resulution = atof(argv[1]);*/
	maxClients = atoi(argv[2]);


	/*************************************

	*	SIGNALS

	*************************************/

	signal(SIGHUP, signalHandler);
	signal(SIGINT, signalHandler);
	signal(SIGKILL, signalHandler);
	signal(SIGQUIT, signalHandler);


	/*************************************

	*	OPENING

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

		*	PREPARE SERVER

		*************************************/

		/*
		*	Active client table reset
		*/
		for (i = 0; i < maxClients; ++i)
			strcpy(activeClientTable[i].pid, "");

		
		/*
		*	Creating FIFOs
		*/
		if ((mkfifo(GTU_PRO_REQ, 0666)) && (errno != EEXIST))
		{
			perror("Error 659");
			exit(EXIT_FAILURE);
		}
		if ((mkfifo(GTU_PRO_REQ, 0666)) && (errno != EEXIST))
		{
			perror("Error 236");
			exit(EXIT_FAILURE);
		}


		/*
		*	Open FIFOs for read and write
		*/
		fdMainConnRequest = open(GTU_PRO_REQ, O_RDONLY);
		fdMainConnResponse = open(GTU_PRO_RES, O_WRONLY);
	}



	/*
	*	Server ON!
	*/

	printf("Active clients %d, Waiting client(s)..\n", activeClients);

	while (1)
	{
		/* Control resets */
		disconnect = 0;

		/*
		*	Waiting a client
		*/
		read(fdMainConnRequest, buffer, BUFFER_SIZE);

		if (strcmp("exit", buffer) == 0)
		{
			printf("Server OFF.\n");
			break;
		}

		else if (strcmp("", buffer) != 0)
		{
			/*
			*	Server capacity control 
			*/
			if (activeClients == maxClients)
			{
				sprintf(buffer, "3");

				/* Server send server full message */
				write(fdMainConnResponse, buffer, BUFFER_SIZE);

				/* Clean buffer data */
				memset(buffer, 0, sizeof(buffer));

				continue;
			}

			/*
			*	Client zaten bagli mi diye kontrol ediyoruz
			*	Eger zaten bagli ise disconnect yollayacagiz illegal bir durum
			*/
			for (i = 0; i < maxClients; ++i)
			{
				if (strcmp(activeClientTable[i].pid, buffer) == 0)
				{
					sprintf(buffer, "9");
					write(fdMainConnResponse, buffer, BUFFER_SIZE);
					deleteClient(activeClientTable[i].pid);
					disconnect = 1;
					break;
				}

				if (strcmp(activeClientTable[i].pid, "") == 0)
					emptySlot = i;
			}


			/*
			*	Client bagli ise bos geciyoruz
			*	Eger bagli degilse register edecegiz
			*/
			if (disconnect == 1)
			{
				continue;
			}
			else
			{
				printf("Client: %s want to connect..\n", buffer);
				addClient(buffer, emptySlot);
				sprintf(buffer, "1");
				write(fdMainConnResponse, buffer, BUFFER_SIZE);

				/*
				*	Server:
				*	create new connection protocol
				*	create pipe for child process communication
				*	create fifo for new client connection
				*	create child process
				*/
				/* Create protocol */
				snprintf(nameNewSecureConnection, 
					GTU_PRO_LEN,
					GTU_PRO_SEC,
					activeClientTable[emptySlot].pid);

				/*
				*	Named pipe
				*	Pipe
				*	Fork
				*/
				mkfifo(nameNewSecureConnection, 0666);
				pipe(pipeDescriptionForServerClient);
				childPid = fork();

				if (childPid == 0)
				{
					int a = 0;

					/* Waiting client connection protocol from server */
					close(pipeDescriptionForServerClient[1]);
					read(pipeDescriptionForServerClient[0], buffer, BUFFER_SIZE);
					close(pipeDescriptionForServerClient[0]);

					strcpy(nameNewSecureConnection, buffer);

					/* Open secure connection */
					fdNewSecureConnection = open(nameNewSecureConnection, O_RDWR);	

					while(1)
					{	
						snprintf(buffer, BUFFER_SIZE, "%d", a++);

						write(fdNewSecureConnection, buffer, BUFFER_SIZE);
						read(fdNewSecureConnection, buffer, BUFFER_SIZE);
						if (strcmp(buffer, "7") == 0)
							break;

						sleep(1);
					}

					close(fdNewSecureConnection);
					unlink(nameNewSecureConnection);
					exit(EXIT_SUCCESS);

				}
				if (childPid > 0)
				{
					/* Parent sending client information with pipe */
					close(pipeDescriptionForServerClient[0]);
					sprintf(buffer, nameNewSecureConnection);
					write(pipeDescriptionForServerClient[1], buffer, BUFFER_SIZE);
					close(pipeDescriptionForServerClient[1]);
					addChild(childPid);
					activeClients++;
					printf("Connection is successful!\n");
					printf("Active clients: %d\n", activeClients);

					/* CONTROL ACTIVE CLIENTS */
					for (int i = 0; i < maxClients; ++i)
					{
						printf("%d. client %s\n", i+1, activeClientTable[i].pid);
					}
				}
				else
				{
					printf("Error 1894: Chil olmuyor tup bebek gerek..\n");
				}

			} /* end if else (Register) */


		} /* end if else (Request control) */

		/* Clean buffer data */
		memset(buffer, 0, sizeof(buffer));
	}

	close(fdMainConnRequest);
	close(fdMainConnResponse);

	unlink(GTU_PRO_REQ);
	unlink(GTU_PRO_RES);
	return 0;

	while(wait(NULL) < 0)
		printf("bekliyor valla amk\n");
}

/*****************************************************

	FUNCTIONS:	Signals

	SUMMERY	

	COMMENTS

*****************************************************/

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

	FUNCTIONS:	Client add and delete

	SUMMERY	

	COMMENTS

*****************************************************/
void addClient(const char clientName[10], const unsigned int i)
{
	strcpy(activeClientTable[i].pid, clientName);
}

void deleteClient(const char clientName[10])
{
	int i;
	for (i = 0; i < maxClients; ++i)
		if (strcmp(activeClientTable[i].pid, clientName))
			strcmp(activeClientTable[i].pid, "");
}

/*****************************************************

	FUNCTIONS:	Child process add and delete

	SUMMERY	

	COMMENTS

*****************************************************/
void addChild(const pid_t childPid)
{
	int i;
	for (i = 0; i < maxClients; ++i)
		if (childPrecessesTable[i] == NULL)
			childPrecessesTable[i] == childPid;
}

void deleteChild(const pid_t childPid)
{
	int i;
	for (i = 0; i < maxClients; ++i)
		if (childPrecessesTable[i] == childPid)
			childPrecessesTable[i] == NULL;
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

