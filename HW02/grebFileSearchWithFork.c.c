#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
	// Using:
	if (argc != 3)
	{
		printf("Using: ./grD [Directory Name] text\n");
		return 1;
	}

	// Folder variables
	DIR *dir;
	struct dirent *ent;
	
	// Try to open folder
	if ((dir = opendir(argv[1])) == NULL) // Argüman olarak değişecek
	{
		perror("opendir");
		return 1;
	}
	else // Dosya başarıyla açılırsa buradan devam edecek.
	{
		// Sırayla dosyları okuyacak.
		while ((ent = readdir(dir)) != NULL)
		{
			pid_t childPid = fork();

			if (childPid < 0) // Child process oluşmaz ise çıkış yapacak o process
			{
				perror("no child\n");
				exit(1);
			}
			else if(childPid == 0) // Başarılı olan processler işleme girecek.
			{
				printf("%s -> \tpid: %d -> ", ent->d_name, getpid());
				exit(0);
			}
			else  // Main (parent) process after fork succeeds 
			{    
			    int returnStatus;    
			    waitpid(childPid, &returnStatus, 0);  // Parent process waits here for child to terminate.

			    if (returnStatus == 0)  // Verify child process terminated without error.  
			    {
			       printf("The child process terminated normally.\n"); 
			    }

			    if (returnStatus == 1)      
			    {
			       printf("The child process terminated with an error!.\n");   
			    }
			} // end else
			
		} // end while readdir

	} // end else

	closedir(dir);
		
	return 0;
}