#include <stdio.h>

#define GTU_PRO_NAM	"Connection"
#define GTU_PRO_SEC	"xL457.GTU-%d.conn"
#define GTU_PRO_LEN	(sizeof(GTU_PRO_SEC) + 10)

struct _request
{
	int pid;

};

struct _response
{
	int identityNo;

};
