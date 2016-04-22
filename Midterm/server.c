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

void addMember(const char clientName[10], const char childName[10], const unsigned int i);
void deleteMember(const int i);

void addChild(const char[10]);
void deleteChild(const char[10]);

/* Signal */
void  signalHandlerServer(int);
void signalHandlerChild(int sign);

/* Other functions */
void openingStyle1(void);
void openingStyle2(void);



/* Global variables */
static struct _childProcessesTable childProcessesTable[255];
static struct _EXCP activeClientTable[255];
static int maxClients;
static int activeClients = 0;




int main(int argc, char const *argv[])
{
	/* Counters */
	int i;

	/* Control variables */
	int disconnect;
	int emptySlot;

	/* Buffers */
	char buffer[BUFFER_SIZE];
	char clientName[10];
	char childName[10];

	/* Pipe variables */
	int pipeDescriptionForServerClient[2];

	/* Fifo variables */
	int fdMainConnRequest;
	int fdMainConnResponse;
	char nameNewSecureConnection[BUFFER_SIZE];
	int fdNewSecureConnection;


	/* Fork variables */
	unsigned long childPid;

	/* Argumans vaariables */
/*	float resulution = atof(argv[1]);*/
	maxClients = atoi(argv[2]);


	/*************************************

	*	SIGNALS

	*************************************/

	signal(SIGHUP, signalHandlerServer);
	signal(SIGINT, signalHandlerServer);
	signal(SIGKILL, signalHandlerServer);
	signal(SIGQUIT, signalHandlerServer);
	signal(SIGUSR1, signalHandlerServer);
	signal(SIGUSR2, SIG_IGN);


	/*************************************

	*	OPENING

	*************************************/
	if ((childPid = fork()) == 0) /* Child */
	{
		openingStyle2();
		exit(EXIT_SUCCESS);
	}
	else /* Main */
	{
		/* Açılışı yapan bay child process must die */
		
		kill(wait(NULL), SIGKILL);

		/*************************************

		*	PREPARE SERVER

		*************************************/

		/*
		*	Active client table reset
		*/
		for (i = 0; i < maxClients; ++i)
			strcpy(activeClientTable[i].pid, "");

		for (i = 0; i < maxClients; ++i)
			strcpy(childProcessesTable[i].childPid, "");

		
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
					deleteMember(i);
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
				strcpy(clientName, buffer);

				/* Sending access */
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
				snprintf(nameNewSecureConnection, GTU_PRO_LEN, GTU_PRO_SEC, clientName);
				strcpy(buffer, nameNewSecureConnection);
				write(fdMainConnResponse, buffer, BUFFER_SIZE);

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

					/*************************************

					*	SIGNALS

					*************************************/

					signal(SIGHUP, signalHandlerServer);
					signal(SIGINT, signalHandlerServer);
					signal(SIGKILL, signalHandlerServer);
					signal(SIGQUIT, signalHandlerServer);
					signal(SIGUSR1, signalHandlerServer);
					signal(SIGUSR2, signalHandlerServer);

					/* Parent sending client information with pipe */
					snprintf(buffer, BUFFER_SIZE, "%lu", (unsigned long)getpid());

					close(pipeDescriptionForServerClient[0]);
					write(pipeDescriptionForServerClient[1], buffer, BUFFER_SIZE);
					close(pipeDescriptionForServerClient[1]);

					/* Open secure connection */
					fdNewSecureConnection = open(nameNewSecureConnection, O_RDWR);	
printf("---%s\n", nameNewSecureConnection);
					/*
					*	Client kayıt etmesi için kendi pid mizi yolluyoruz
					*/
					write(fdNewSecureConnection, buffer, BUFFER_SIZE);

					while(1)
					{	
						snprintf(buffer, BUFFER_SIZE, "%d", a++);

						write(fdNewSecureConnection, buffer, BUFFER_SIZE);

						sleep(2);
					}

					close(fdNewSecureConnection);
					unlink(nameNewSecureConnection);
					exit(EXIT_SUCCESS);

				}
				if (childPid > 0)
				{

					/* Parent sending client information with pipe */
					close(pipeDescriptionForServerClient[1]);
					read(pipeDescriptionForServerClient[0], buffer, BUFFER_SIZE);
					close(pipeDescriptionForServerClient[0]);

					strcpy(childName, buffer);

					addMember(clientName, childName, emptySlot);
					activeClients++;

					snprintf(buffer, 10, "%lu", childPid);
					strcpy(activeClientTable[emptySlot].childPid, buffer);

					printf("Connection is successful!\n");
					printf("Active clients: %d\n", activeClients);


					/* CONTROL ACTIVE CLIENTS */
					for (i = 0; i < maxClients; ++i)
					{
						printf("%d. Client %s\n", i+1, activeClientTable[i].pid);
					}
					for (i = 0; i < maxClients; ++i)
					{
						printf("%d. Child  %s\n", i+1, childProcessesTable[i].childPid);
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
}

/*****************************************************

	FUNCTIONS:	Signals

	SUMMERY	

	COMMENTS

*****************************************************/

void signalHandlerServer(int sign)
{
	int i;
	char childName[10];

	if (sign == SIGHUP || sign == SIGINT || sign == SIGKILL || sign == SIGQUIT)
	{
		exit(EXIT_SUCCESS);
	}

	if (sign == SIGUSR1)
	{	
		printf("Parent SIGUSR1 yakaladı\n");

		pid_t childPid = waitpid(-1, NULL, 0);
		
		snprintf(childName, 10, "%lu", (unsigned long)childPid);

		for (i = 0; i < maxClients; ++i)
			if (strcmp(activeClientTable[i].childPid, childName) == 0)
			{
				printf("Merhaba\n");
				deleteMember(i);

				activeClients--;
				break;
			}

		printf("Active clients: %d\n", activeClients);

		/* CONTROL ACTIVE CLIENTS */
		for (i = 0; i < maxClients; ++i)
		{
			printf("%d. Client %s\n", i+1, activeClientTable[i].pid);
		}
		for (i = 0; i < maxClients; ++i)
		{
			printf("%d. Child  %s\n", i+1, childProcessesTable[i].childPid);
		}

	}


}

void signalHandlerChild(int sign)
{
	char childName[10];

	if (sign == SIGHUP || sign == SIGINT || sign == SIGKILL || sign == SIGQUIT)
	{
		for (int i = 0; i < maxClients; ++i)
			kill(atoi(activeClientTable[i].pid), SIGUSR2);

		exit(EXIT_SUCCESS);
	}


	if (sign == SIGUSR1)
	{
		printf("Parent mesaj yolladı\n");
		snprintf(childName, 10, "%lu", (unsigned long)getpid());

		for (int i = 0; i < maxClients; ++i)
		{
			if (strcmp(activeClientTable[i].childPid, childName) == 0)
			{
				kill(atoi(activeClientTable[i].pid), SIGUSR2);
			}
		}

		exit(EXIT_SUCCESS);
	}

	if (sign == SIGUSR2)
	{
		printf("Child SIGUSR2 yakaladı\n");
		kill(getppid(), SIGUSR1);
		exit(EXIT_SUCCESS);
	}
}


/*****************************************************

	FUNCTIONS:	Client add and delete

	SUMMERY	

	COMMENTS

*****************************************************/
void addMember(const char clientName[10], const char childName[10], const unsigned int i)
{
	strcpy(activeClientTable[i].pid, clientName);
	strcpy(activeClientTable[i].childPid, childName);
	strcpy(childProcessesTable[i].childPid, childName);
}
	
void deleteMember(const int i)
{
	strcpy(activeClientTable[i].pid, "");
	strcpy(activeClientTable[i].childPid, "");
	strcpy(childProcessesTable[i].childPid, "");
}

/*****************************************************

	FUNCTIONS:	Child process add and delete

	SUMMERY	

	COMMENTS

*****************************************************/
void addChild(const char childPid[10])
{
	int i;
	for (i = 0; i < maxClients; ++i)
		if(strcmp(childProcessesTable[i].childPid, "") == 0)
		{
			strcpy(childProcessesTable[i].childPid, childPid);
			break;
		}

	/* Control child status */
	for (i = 0; i < maxClients; ++i)
	{
		printf("Child durum:  %s\n", childProcessesTable[i].childPid);
	}
}

void deleteChild(const char childPid[10])
{
	int i;
	for (i = 0; i < maxClients; ++i)
		if(strcmp(childProcessesTable[i].childPid, childPid) == 0)
			strcpy(childProcessesTable[i].childPid, "");
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

