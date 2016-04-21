#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define BUFFER_SIZE 100

#define GTU_PRO_REQ	"Request"
#define GTU_PRO_RES	"Respond"
#define GTU_PRO_SEC	"xL457.GTU-%ld.conn"
#define GTU_PRO_LEN	(sizeof(GTU_PRO_SEC) + 10)


struct _EXCP	/* (Ex)tended (C)onnection (P)rotocol */
{
	pid_t pid;
	char identity[GTU_PRO_LEN];
	int status;
	char buffer[BUFFER_SIZE];
};

struct _CALP	/* (C)alculate (A)rguman (L)ist (P)rotocol */
{
	pid_t pid;
	char fi[2];
	char fj[2];
	float timeInterval;
	char operand;
};


/*
Status codes:

-1	Client zaten kayitli
0	Ilk baglanti istegi
1	Register edildi sorun yok
2	Server send new data to client
3	Client read new data from server
*/