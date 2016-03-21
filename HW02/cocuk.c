#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <limits.h>



void findDirectoryAndFiles(const char *searchDirchar,const char *searchingString);

int main(int argc, char const *argv[])
{
	if (argc != 3)
	{
		fprintf(stderr,"Usage: ./grepFromDir directoryName searchingString\n");
		return 1;
	}
	findDirectoryAndFiles(argv[1],argv[2]);
	
	return 0;
}

void findDirectoryAndFiles(const char *searchDir,const char *searchingString)
{
	DIR *mydir;
	struct dirent *direntp;
	
	if ((mydir = opendir(searchDir)) == NULL)
	{	
		perror("opendir");
		exit(EXIT_FAILURE);
	
	}
	else //dosya acilmadi ise hata ver ve cik
	{
		while((direntp = readdir(mydir)) != NULL)
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
				printf("chil: %d -> parent: %d\n%s\n\n", getpid(), getppid(), direntp->d_name);
			}
			else  // Parent process after fork succeeds 
			{    
			    wait(NULL);
			} // end else
			
		}
	}
	closedir(mydir);
}

