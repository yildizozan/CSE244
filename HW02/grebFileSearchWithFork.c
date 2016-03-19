#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>

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
		while (errno = 0, (ent = readdir(dir)) != NULL)
		{
			pid_t childPid = fork();

			// Önce hata kontrolü sağlıyoruz.
			if (errno)
			{
				perror("readdir");
				exit(EXIT_FAILURE);
			}

			if (childPid < 0) // Child process oluşmaz ise çıkış yapacak o process
			{
				perror("no child\n");
				exit(EXIT_FAILURE);
			}
			else if(childPid == 0) // Başarılı olan processler işleme girecek.
			{
				if(strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
					exit(0);

				

				printf("%s -> \tpid: %d -> parent: %d\n", ent->d_name, getpid(), getppid());
				exit(EXIT_FAILURE);
			}
			else  // Parent process after fork succeeds 
			{    
			    wait(NULL);
			} // end else
			
		}

	} // end else

	closedir(dir);
}