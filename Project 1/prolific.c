#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>

#include "util_random.h"

int main(int argc, char* argv[]) {
	int seed = 0;
	int lowerLimit = 10;
	int upperLimit = 15;
	
	read_seed("seed.txt", &seed);
	printf("Read seed value %i\n", seed);
	
	srand(seed);

	int randomNumber = get_random_number(lowerLimit, upperLimit);
	printf("Random Child Count: %i\n", randomNumber);
	
	
	printf("I'm feeling prolific!\n");
	int childNumbers[randomNumber];
	for (int i = 0; i < randomNumber; ++i) {
		childNumbers[i] = rand();
		
		int rc = fork(); 
		if ( rc < 0 ) {
			printf("fork failed!\n");
			exit(1);
		} else if (rc == 0) {
			//child is this code
			int pid = (int) getpid();
			int exitCode = (childNumbers[i] % 50 ) + 1;
			int waitTime = (childNumbers[i] % 3) + 1;
			printf("\t[Child, PID: %i]: I am the child and I will wait %i seconds and exit with code %i.\n"
					, pid, waitTime,  exitCode);
			sleep(waitTime);
			printf("\t[Child, PID: %i]: Now exiting...\n", pid); 	
			return exitCode;
				
			
		} else {
			//parent is this code
			int exitCode;
			printf("[Parent]: I am waiting for PID %i to finish.\n", rc);
			waitpid(rc, &exitCode, 0);
			printf("[Parent]: Child %i finished with status code %i. Onward!\n", rc, WEXITSTATUS(exitCode));
			
		}	
	}
	

	return 0;
}






