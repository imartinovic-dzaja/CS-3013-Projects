#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>
#include <time.h>

void read_seed(char* filepath, int* seed);
int get_random_number(int lowerLimit, int upperLimit);

int main(int argc, char* argv[]) {
	
	int numOfChildren = 4;
	int childPID[4];
	for (int i = 0; i < numOfChildren; ++i) {
		childPID[i] = fork(); 
		
		if ( childPID[i] < 0 ) {
			printf("fork failed!\n");
			exit(1);
		} else if (childPID[i] == 0) {
			//child is this code

			int pid = (int) getpid();
			printf("\t[Child, PID: %i]: Executing './slug %i' command ...\n", pid, i);
			
			char *args[3];
			char fileNumString[2];
			fileNumString[0] = (char) i + 1 + '0';
			fileNumString[1] = '\0';

 			 args[0] = "./slug";     
   			 args[1] = fileNumString;     
   			 args[2] = NULL;
   			 
			if (execvp(args[0], args) == -1) {
				printf("Error: Most Likely slug executable is missing\n");
				exit(1);
			}
			
		} else {
			printf("[Parent]: I forked off child: %i.\n", childPID[i]);
		}		
	}
	// only parent will execute this
	int pid;
	struct timespec startTime;
	struct timespec currentTime;
	//struct timespec tempTime;
	clock_gettime(CLOCK_MONOTONIC, &startTime);
	int numOfActiveChildren = numOfChildren;
	while(numOfActiveChildren > 0) {
		pid = waitpid(-1, NULL, WNOHANG);
		if(pid > 0) {	// a child has finished the race
			clock_gettime(CLOCK_MONOTONIC, &currentTime);
			int numSeconds = currentTime.tv_sec - startTime.tv_sec;
			long nanoSeconds = currentTime.tv_nsec - startTime.tv_nsec;
			printf("[Parent]: Child %i has crossed the finish line! It took %f seconds\n", 
						pid, numSeconds + (double) nanoSeconds / 1000000000ul);
			
			for (int j = 0; j < numOfChildren; ++j) {
				if(childPID[j] == pid) {
					childPID[j] = -1;	//indicate that the childPID terminated
					break;
				}
			}
			
			--numOfActiveChildren;
			if(numOfActiveChildren  == 0)  {
				printf("[Parent]: The race is over. It took %f seconds\n", numSeconds + (((double) nanoSeconds) /1000000000ul));
				break;
			}
			
		}else{
			usleep(250000);
			printf("The race is ongoing. The following children are still racing:");
			for (int j = 0; j < numOfChildren; ++j) {
					if (childPID[j] > 0) {printf("%i ", childPID[j]);}	//print active childPID's
				}
			printf("\n");

		}
					 		
	}

	return 0;
}






