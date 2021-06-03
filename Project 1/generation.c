#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>

#include "util_random.h"

int main(int argc, char* argv[]) {
	int seed = 0;
	int lowerLimit = 7;
	int upperLimit = 11;
	
	read_seed("seed.txt", &seed);
	printf("Read seed value %i\n", seed);
	
	srand(seed);
	int lifeSpanCount = get_random_number(lowerLimit, upperLimit);
	printf("Random Descendant Count: %i\n", lifeSpanCount);
	printf("Time to meet the kids/grandkids/great grand kids/...\n");
	while (lifeSpanCount > 0) {
		
		int rc = fork(); 
		if (lifeSpanCount < 0) {exit(1);};
		if ( rc < 0 ) {
			printf("fork failed!\n");
			exit(1);
		} else if (rc == 0) {
			//child is this code
			int pid = (int) getpid();
			printf("\t[Child, PID: %i]: I was called with descendant count = %i. I'll have %i descendant(s).\n"
					, pid, lifeSpanCount,  lifeSpanCount - 1);
			lifeSpanCount--;
				
			
		} else {
			//parent is this code
			int exitCode;
			printf("[Parent]: I am waiting for PID %i to finish.\n", rc);
			waitpid(rc, &exitCode, 0);
			printf("[Parent]: Child %i finished with status code %i. It's now my turn to exit.!\n", rc, WEXITSTATUS(exitCode));
			break;		//without this the code would enter an infinite loop since lifeSpanCount for parent never decreases
			
		}	
	} 
	
	return lifeSpanCount;
}
