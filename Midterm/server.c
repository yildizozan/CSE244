/***
 *    ______/\\\___/\\\\\\\\\\\\\\\_______/\\\_____/\\\\\\\_______________/\\\_______________/\\\________/\\\\\\\________/\\\\\\\\\_______/\\\\\\\\\_____        
 *     __/\\\\\\\__\/\\\///////////____/\\\\\\\___/\\\/////\\\___________/\\\\\_____________/\\\\\______/\\\/////\\\____/\\\///////\\\___/\\\///////\\\___       
 *      _\/////\\\__\/\\\______________\/////\\\__/\\\____\//\\\________/\\\/\\\___________/\\\/\\\_____/\\\____\//\\\__\/\\\_____\/\\\__\///______\//\\\__      
 *       _____\/\\\__\/\\\\\\\\\\\\_________\/\\\_\/\\\_____\/\\\______/\\\/\/\\\_________/\\\/\/\\\____\/\\\_____\/\\\__\///\\\\\\\\\/_____________/\\\/___     
 *        _____\/\\\__\////////////\\\_______\/\\\_\/\\\_____\/\\\____/\\\/__\/\\\_______/\\\/__\/\\\____\/\\\_____\/\\\___/\\\///////\\\_________/\\\//_____    
 *         _____\/\\\_____________\//\\\______\/\\\_\/\\\_____\/\\\__/\\\\\\\\\\\\\\\\__/\\\\\\\\\\\\\\\\_\/\\\_____\/\\\__/\\\______\//\\\_____/\\\//________   
 *          _____\/\\\__/\\\________\/\\\______\/\\\_\//\\\____/\\\__\///////////\\\//__\///////////\\\//__\//\\\____/\\\__\//\\\______/\\\____/\\\/___________  
 *           _____\/\\\_\//\\\\\\\\\\\\\/_______\/\\\__\///\\\\\\\/_____________\/\\\______________\/\\\_____\///\\\\\\\/____\///\\\\\\\\\/____/\\\\\\\\\\\\\\\_ 
 *            _____\///___\/////////////_________\///_____\///////_______________\///_______________\///________\///////________\/////////_____\///////////////__
 */

#include "protocol.h"

void addClient(const char clientName[BUFFER_SIZE], const unsigned int i);
void deleteClient(const char clientName[BUFFER_SIZE]);

void addChild(const pid_t childPid);
void deleteChild(const pid_t childPid);

/* Signal */
void  signalHanflerSIGINT(int);
void  signalHanflerSIGQUIT(int);

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

	signal(SIGINT, signalHanflerSIGINT);


	/*************************************

	*	OPENING

	*************************************/
	if (fork() == 0) /* Child */
	{
		openingStyle2();
		exit(EXIT_SUCCESS);
	}
	else /* Main */
	{
		/*************************************

		*	PREPARE SERVER

		*************************************/
		/* Table reset */
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



	printf("Active clients %d, Waiting client(s)..\n", activeClients);
	/*
	*	Server ON!
	*/
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

				/* Create pipe */
				pipe(pipeDescriptionForServerClient);
					perror("Child pipe status");

				mkfifo(nameNewSecureConnection, 0666);
					perror("Child fifo status");

				childPid = fork();
					perror("Child fork status");

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

void signalHanflerSIGINT(int sign)
{

	if(sign == SIGINT)

		printf("Ctrl + C yakalandi\n");
		exit(1);

/*
		for (i = 0; i < maxClients; ++i)
		{
			activeClientTable[i].pid
		}
*/
}

void signalHanflerSIGQUIT(int sign)
{
	int i;
	if(sign == SIGINT)
		for (i = 0; i < maxClients; ++i)
		{
			printf("Ctrl + C yakalandi\n");
		}
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

}

