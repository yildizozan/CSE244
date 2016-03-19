#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/wait.h>

int searching(char *directory);

int main(int argc, char *argv[])
{
	// Using:
	if (argc != 3)
	{
		printf("Using: ./grD [Directory Name] text\n");
		return 1;
	}

	searching(argv[1]);
		
	return 0;
}

int searching (char *directory)
{
	// Folder variables
	DIR *dir;
	struct dirent *ent;
	
	// Try to open folder
	if ((dir = opendir(directory)) == NULL) // Argüman olarak değişecek
	{
		perror("opendir");
		return 1;
	}
	else // Dosya başarıyla açılırsa buradan devam edecek.
	{
		// Eğer directory açılırsa işlemleri yapacak.
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
				printf("%s -> \tpid: %d -> parent: %d\n", ent->d_name, getpid(), getppid());
				exit(0);
			}
			else  // Parent process after fork succeeds 
			{    
			    wait(NULL);
			} // end else
			
		}

	} // end else

	closedir(dir);
}