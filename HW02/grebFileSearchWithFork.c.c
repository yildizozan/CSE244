#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/stat.h>

int searching(char *currentPath);

int main(int argc, char *argv[])
{
	// Using:
	if (argc != 3)
	{
		printf("Using: ./grD [currentPath Name] text\n");
		return 1;
	}

	searching(argv[1]);
		
	return 0;
}

int searching (char *currentPath)
{
	// Folder variables
	DIR *dir;
	struct dirent *ent;
	
	// Try to open folder
	if ((dir = opendir(currentPath)) == NULL) // Argüman olarak değişecek
	{
		perror("opendir");
		return 1;
	}
	else // Dosya başarıyla açılırsa buradan devam edecek.
	{
		// Eğer currentPath açılırsa işlemleri yapacak.
		while ((ent = readdir(dir)) != NULL)
		{
			pid_t childPid = fork();

			// Child process oluşmaz ise çıkış yapacak o process
			if (childPid < 0)
			{
				perror("no child\n");
				exit(EXIT_FAILURE);
			}
			else if(childPid == 0) // Başarılı olan processler işleme girecek.
			{
				// Geçerli dizin tekrardan ve üst dizini değerlendirmeye alınmayacak.
				if(strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
					exit(0);

				// Klasör olduğuna denk gelirsek recursive takılıyoruz.
				if (S_ISDIR(currentPath) == 0)
				{
					char *newPath;
					newPath = currentPath;
					strcat(newPath, "/");
					strcat(newPath, ent->d_name);
					searching (newPath);
				}
				else
				{
					printf("%s -> \tpid: %d -> parent: %d\n", ent->d_name, getpid(), getppid());
					exit(EXIT_FAILURE);
				}
			}
			else  // Parent process after fork succeeds 
			{    
			    wait(NULL);
			} // end else
			
		}

	} // end else

	closedir(dir);
}
// NOTLAR
// ent değişkeni değişecek