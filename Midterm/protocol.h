#include <stdio.h>

#define BUFFER_SIZE 4096

#define GTU_PRO_NAM	"Connection"
#define GTU_PRO_SEC	"xL457.GTU-%ld.conn"
#define GTU_PRO_LEN	(sizeof(GTU_PRO_SEC) + 10)

struct _request
{
	pid_t pid;
};
struct _response
{
	pid_t pid;
	char identity[GTU_PRO_LEN];
	int status;
};
struct _CAL /* (C)alculate (A)rguman (L)ist */
{
	pid_t pid;
	char fi[2];
	char fj[2];
	float timeInterval;
	char operand;
};
struct _conn
{
	pid_t pid;
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