#include "protocol.h"

void addClient(const char clientName[BUFFER_SIZE], const unsigned int i);
void deleteClient(const char clientName[BUFFER_SIZE]);


/* Global variables */
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

	/* Fifo variables */
	int fdMainConnRequest;
	int fdMainConnResponse;

	/* Argumans vaariables */
/*	float resulution = atof(argv[1]);*/
	maxClients = atoi(argv[2]);

	/*************************************

	*	OPENING

	*************************************/
printf("\n");
printf(".d8888. d88888b d8888b. db    db d88888b d8888b.      db    db  db\n");
printf("88'  YP 88'     88  `8D 88    88 88'     88  `8D      88    88 o88\n");
printf("`8bo.   88ooooo 88oobY' Y8    8P 88ooooo 88oobY'      Y8    8P  88\n");
printf("  `Y8b. 88~~~~~ 88`8b   `8b  d8' 88~~~~~ 88`8b        `8b  d8'  88\n");
printf("db   8D 88.     88 `88.  `8bd8'  88.     88 `88.       `8bd8'   88\n");
printf("`8888Y' Y88888P 88   YD    YP    Y88888P 88   YD         YP     VP\n");
printf("\n");
printf("                                            Created by Ozan Yıldız\n");
printf("SERVER ON!..\n");


printf(" _______  _______  ______    __   __  _______  ______      __   __  ____\n");
printf("|       ||       ||    _ |  |  | |  ||       ||    _ |    |  | |  ||    |\n");
printf("|  _____||    ___||   | ||  |  |_|  ||    ___||   | ||    |  |_|  | |   |\n");
printf("| |_____ |   |___ |   |_||_ |       ||   |___ |   |_||_   |       | |   |\n");
printf("|_____  ||    ___||    __  ||       ||    ___||    __  |  |       | |   |\n");
printf(" _____| ||   |___ |   |  | | |     | |   |___ |   |  | |   |     |  |   |\n");
printf("|_______||_______||___|  |_|  |___|  |_______||___|  |_|    |___|   |___|\n");
printf("\n");
printf("                                                   Created by Ozan Yıldız\n");
printf("SERVER ON!..\n");



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
					sprintf(buffer, "-1");
					write(fdMainConnResponse, buffer, BUFFER_SIZE);
					disconnect = 1;
					break;
				}

				if (strcmp(activeClientTable[i].pid, "") == 0)
				{
					emptySlot = i;
				}
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
				sprintf(buffer, "2");
				write(fdMainConnResponse, buffer, BUFFER_SIZE);
			}


		}

		/* clean buffer from any data */
		memset(buffer, 0, sizeof(buffer));
	}

	close(fdMainConnRequest);
	close(fdMainConnResponse);

	unlink(GTU_PRO_REQ);
	unlink(GTU_PRO_RES);
	return 0;
}

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










