#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/stat.h>

void searching(char *currentPath);

int main(int argc, char *argv[])
{
	// Using:
	if (argc != 3)
	{
		printf("Using: ./grD [directory] text\n");
		return 1;
	}

	searching(argv[1]);
		
	return 0;
}

void searching (char *currentPath)
{
	// Folder variables
	DIR *dir;
	struct dirent *ent;
	
	// Try to open folder
	if ((dir = opendir(currentPath)) == NULL)
	{
		perror("opendir");
		exit(EXIT_FAILURE);
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
					exit(EXIT_FAILURE);

				// Control for folder
				if (ent->d_type == DT_DIR)
				{
					// Create new path
					char *newPath = currentPath;
					strcat(newPath, "/");
					strcat(newPath, ent->d_name);

					// Searching new path
					searching(newPath);
					return;
				}
				else if(ent->d_type == DT_REG) // Control for file
				{
					printf("%s -> \tpid: %d -> parent: %d\n", ent->d_name, getpid(), getppid());
					return;
				}
			}
			else  // Parent process after fork succeeds 
			{    
			    wait(NULL);
			} // end else
			
		}
		closedir(dir);

	} // end else

}

// NOTLAR
// ent değişkeni değişecek