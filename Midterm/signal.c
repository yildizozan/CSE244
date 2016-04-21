#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void handler(int sigtype)
{
	printf("Selam lan %d\n", sigtype);
}

int main(int argc, char const *argv[])
{
	struct sigaction action;

	int a;

	action.sa_handler = handler;
	sigemptyset.(&action.sa_mask);
	action.sa_flags = 0;
	sigaction(SIGINT, &action, NULL);

/*	
	int childpid;

for (int i = 0; i < 10; ++i)
	{
		childpid= fork();

		if (childpid == 0)
		{
			while(1)
			{

			}
		}
	}
*/

	while (1)
	{
		printf("%d\n", a++);
	}

	return 0;
}

