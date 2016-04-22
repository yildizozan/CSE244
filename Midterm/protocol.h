#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

#define BUFFER_SIZE 100

#define GTU_PRO_NAM "Connection"
#define GTU_PRO_SEC	"xL457.GTU-%ld.conn"
#define GTU_PRO_LEN	(sizeof(GTU_PRO_SEC) + 10)

struct _EXCP	/* (Ex)tended (C)onnection (P)rotocol */
{
	pid_t pidClient;
	pid_t pidChild;
	char identity[GTU_PRO_LEN];
	char data[BUFFER_SIZE];
	int status;
};

struct _CALP	/* (C)alculate (A)rguman (L)ist (P)rotocol */
{
	unsigned long pid;
	char fi[2];
	char fj[2];
	float timeInterval;
	char operand;
};

/*
Status codes:

-1	Server full
-----------------
0	Ilk baglanti istegi
1	Register edildi sorun yok

4	Server send new data to client
5	Client read new data from server
7	Client kapaniyor.
8	Server kapaniyor. Disconnect.
9	Client zaten kayitli disconnect.
*/