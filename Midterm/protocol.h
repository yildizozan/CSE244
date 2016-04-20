#include <stdio.h>

#define BUFFER_SIZE 4096

#define GTU_PRO_NAM	"Connection"
#define GTU_PRO_SEC	"xL457.GTU-%ld.conn"
#define GTU_PRO_LEN	(sizeof(GTU_PRO_SEC) + 10)

struct _request
{
	unsigned long pid;
};
struct _response
{
	char identity[GTU_PRO_LEN];
	unsigned long pid;
	int status;
};
struct _CAL /* (C)alculate (A)rguman (L)ist */
{
	unsigned long pid;
	char fi[2];
	char fj[2];
	float timeInterval;
	char operand;
};
struct _conn
{
	unsigned long pid;
	char buffer[BUFFER_SIZE];
	int status;
};

/*
Status codes:

-1	Client zaten kayitli
0	Ilk baglanti istegi
1	Register edildi sorun yok
2	Server send new data to client
3	Client read new data from server
*/