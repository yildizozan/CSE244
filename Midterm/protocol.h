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
	char pid[10];
	char identity[GTU_PRO_LEN];
	char status;
	char message[BUFFER_SIZE];
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

0	Client zaten kayitli
1	Ilk baglanti istegi
2	Register edildi sorun yok
4	Server send new data to client
5	Client read new data from server
*/