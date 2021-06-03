#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>

#include "util_random.h"

int main(int argc, char* argv[]) {
	int seed = 0;
	int numOfSelections = 5;
	int numOfDirectories = 6;
	
	const char* directories[] = {
		"/home",
		"/proc",
		"/proc/sys",
		"/usr",
		"/usr/bin",
		"/bin"
	};
	
	
	read_seed("seed.txt", &seed);
	printf("Read seed value %i\n", seed);
	
	srand(seed);
	
	
	printf("It's time to see the world/file system!\n");

	for (int i = 0; i < numOfSelections; ++i) {

		int randomNumber = rand() % 6;
		printf("Random number is %i\n", randomNumber);
		printf("Selection #%i: %s [SUCCESS]\n", i+1, directories[randomNumber]);
		chdir(directories[randomNumber]);
		char cwd[32];
		printf("Current reported directory: %s\n", getcwd(cwd, 32));
		
		int rc = fork(); 
		if ( rc < 0 ) {
			printf("fork failed!\n");
			exit(1);
		} else if (rc == 0) {
			//child is this code

			int pid = (int) getpid();

			char *args[3];


 			 args[0] = "ls";     
   			 args[1] = "-alh";     
   			 args[2] = NULL;
   			 

			printf("\t[Child, PID: %i]: Executing 'ls -alh' command...\n", pid);
			execvp(args[0], args);
			printf("\t[Child, PID: %i]: Now exiting...\n", pid); 	
			return 0;
				
			
		} else {
			
			int exitCode;
			printf("[Parent]: I am waiting for PID %i to finish.\n", rc);
			waitpid(rc, &exitCode, 0);
			printf("[Parent]: Child %i finished with status code %i. Onward!\n", rc, WEXITSTATUS(exitCode));
			
		}	
	}
	

	return 0;
}


