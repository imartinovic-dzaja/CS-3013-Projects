#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>

#include "util_random.h"

int main(int argc, char* argv[]) {
	int seed = 0;
	int pid = (int) getpid();
	int lowerLimitSeconds = 1;
	int upperLimitSeconds = 5;
	int lowerLimitCoin = 0;
	int upperLimitCoin = 1;

	if (argc != 2) {
		printf("Must specify which seed_slug_x.txt in arguments\n");
		printf("Currently num of args is: %i\n", argc);
		exit(-1);
	}
	int file_number = (int) (argv[1][0] - '0');
	char txt_file[] = "seed_slug_0.txt";
	switch (file_number) {
		case 1:
			txt_file[10] = '1';
			break;
		case 2:
			txt_file[10] = '2';
			break;
		case 3:
			txt_file[10] = '3';
			break;
		case 4:
			txt_file[10] = '4';
			break;
		default:
			printf("File # out of bounds!\n");
			printf("File num is: char: %s, int: %i\n", argv[1], file_number); 
			exit(-2);
	} 
	
	read_seed(txt_file, &seed);
	printf("[Slug PID: %i] Read seed value: %i\n",pid, seed);
	srand(seed);
	
	int num_of_seconds = get_random_number(lowerLimitSeconds, upperLimitSeconds);
	int coin_flip = get_random_number(lowerLimitCoin, upperLimitCoin);
	printf("[Slug PID: %i] Delay time is %i seconds. Coin flip: %i\n",pid, num_of_seconds, coin_flip);
	printf("[Slug PID: %i] I'll get the job done. Eventually ...\n",pid);
	sleep(num_of_seconds);

	if (coin_flip) {
		printf("[Slug PID: %i] Break time is over! I am running the 'id -u' command.\n", pid);
		
		char *args[3];

 		args[0] = "id";      
   		args[1] = "-u";      
   		args[2] = NULL;
   			 

		execvp(args[0], args);
			
	} else {
		printf("[Slug PID: %i] Break time is over! I am running the 'last -d --fulltimes' command.\n", pid);	
		char *args[4];

 		args[0] = "last";      
   		args[1] = "-d";      
   		args[2] = "--fulltimes";
   		args[3] = NULL;	 

		execvp(args[0], args);
	}
	
	return 0;
}
