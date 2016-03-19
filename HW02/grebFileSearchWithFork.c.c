#include <stdlib.h>
  #include <dirent.h>		  #include <dirent.h>
  #include <unistd.h>		  #include <unistd.h>
 +#include <sys/wait.h>
  		  
  int main(int argc, char *argv[])		  int main(int argc, char *argv[])
  {		  {
 @@ -27,24 +28,33 @@ int main(int argc, char *argv[])
  		// Sırayla dosyları okuyacak.		  		// Sırayla dosyları okuyacak.
  		while ((ent = readdir(dir)) != NULL)		  		while ((ent = readdir(dir)) != NULL)
  		{		  		{
 -			pid_t childpid = fork();		 +			pid_t childPid = fork();
  		  
 -			if (childpid < 0) // Child process oluşmaz ise çıkış yapacak o process		 +			if (childPid < 0) // Child process oluşmaz ise çıkış yapacak o process
  			{		  			{
  				perror("no child\n");		  				perror("no child\n");
 -				exit (EXIT_FAILURE);		 +				exit(1);
  			}		  			}
 -			else if(childpid == 0) // Başarılı olan processler işleme girecek.		 +			else if(childPid == 0) // Başarılı olan processler işleme girecek.
  			{		  			{
 -				printf("alt process\n");		 +				printf("%s -> \tpid: %d -> ", ent->d_name, getpid());
 -				//printf("%s\n", ent->d_name);		 +				exit(0);
 -				return 1;		
 -			}		
 -			else		
 -			{		
 -				int returnStatus;		
 -				wait(NULL);		
  			}		  			}
 +			else  // Main (parent) process after fork succeeds 
 +			{    
 +			    int returnStatus;    
 +			    waitpid(childPid, &returnStatus, 0);  // Parent process waits here for child to terminate.
 +
 +			    if (returnStatus == 0)  // Verify child process terminated without error.  
 +			    {
 +			       printf("The child process terminated normally.\n");       
 +			    }
 +
 +			    if (returnStatus == 1)      
 +			    {
 +			       printf("The child process terminated with an error!.\n");   
 +			    }
 +			} // end else
  					  			
  		} // end while readdir
 } // end while readdir