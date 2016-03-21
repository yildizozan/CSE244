#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>


void findDirectoryAndFiles( char *searchDirchar, char *searchingString);
void fileCheck (char *currentPath, char *searchText);

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		fprintf(stderr,"Usage: ./grepFromDir directoryName searchingString\n");
		return 1;
	}
	fileCheck(argv[1],argv[2]);
	
	return 0;
}

void findDirectoryAndFiles(char *searchDir, char *searchingString)
{
	DIR *mydir;
	struct dirent *ent;
	
	if ((mydir = opendir(searchDir)) == NULL)
	{	
		perror("opendir");
		exit(EXIT_FAILURE);
	
	}
	else //dosya acilmadi ise hata ver ve cik
	{
		while((ent = readdir(mydir)) != NULL)
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
				printf("chil: %d -> parent: %d\n%s\n\n", getpid(), getppid(), ent->d_name);
			}
			else  // Parent process after fork succeeds 
			{    
			    wait(NULL);
			} // end else
			
		}
		closedir(mydir);
	}
	
}

void fileCheck (char *currentPath, char *searchText)
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
	else // Successfull open dir
	{
		// Reading directioary
		while ((ent = readdir(dir)) != NULL)
		{
			pid_t childPid = fork();

			// Child process can't be created
			if (childPid < 0)
			{
				perror("no child\n");
				exit(EXIT_FAILURE);
			}
			else if(childPid == 0) // Process is success
			{
				// NO CWD and upper
				if(strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
					exit(EXIT_FAILURE);

				// Control for folder
				if (ent->d_type == DT_DIR)
				{
					// Create new path
					char newPath[255];
					strcpy(newPath, currentPath);
					strcat(newPath, "/");
					strcat(newPath, ent->d_name);

					// Filecheck new path
					fileCheck(newPath, searchText);
					return;
				}
				// Control for file
				else if(ent->d_type == DT_REG)
				{
					// File path
					char tempPath[255];
					strcpy(tempPath, currentPath);
					strcat(tempPath, "/");
					strcat(tempPath, ent->d_name);

					// Searching new path
					searching(tempPath, searchText);

					// File name print
					printf("%s dosyası tarandı.\n", ent->d_name);
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

} // end function
