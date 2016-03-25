#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char const *argv[])
{
	int fileDescription[2];

	if (pipe(fileDescription) < 0)
	{
		perror("pipe");
		exit(EXIT_FAILURE);
	} // end if (pipe(fileDescription) < 0)
	else
	{
		// Create process
		pid_t pid = fork();

		if (pid < 0) // Process can't create
		{
			perror("fork");
			exit(EXIT_FAILURE);

		} // end if (pid < 0)
		else if (pid == 0) // Child process
		{
			close(fileDescription[1]); // Childprocess close for write

			int result, val;
			while ((result = read(fds[0], &val, sizeof(int))) < 0)
			{

			}
			if (result < 0)
			{
				perror("read");
				exit(EXIT_FAILURE);
			}

			close(fileDescription[0]); // Read complete

		} // end else if (pid == 0)
		else // Parent process
		{
			close(fileDescription[0]); // Parent process close for read

			int i;
			for (i = 0; i < 100; ++i)
			{
				if (write(fds[1], &i, sizeof(int)) < 0)
				{
					perror("write");
					exit(EXIT_FAILURE);
				}
			}
			close(fileDescription[1]); // Writing complete

		} // end else

	} // end else (pipe(fileDescription) < 0)

	return 0;
}