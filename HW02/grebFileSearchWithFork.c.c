#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>

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
			pid_t childpid = fork();

			if (childpid < 0) // Child process oluşmaz ise çıkış yapacak o process
			{
				perror("no child\n");
				exit (EXIT_FAILURE);
			}
			else if(childpid == 0) // Başarılı olan processler işleme girecek.
			{
				printf("alt process\n");
				//printf("%s\n", ent->d_name);
				return 1;
			}
			else
			{
				int returnStatus;
				wait(NULL);
			}
			
		} // end while readdir

	} // end else

	closedir(dir);
		
	return 0;
}