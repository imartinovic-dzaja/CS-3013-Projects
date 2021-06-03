#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "include.h"

#define policy_len 4

char* policies[] = {
	"FIFO",
	"SJF",
	"RR"	
};



int fifo();
int sjf();
int rr();
int testList();

int main(int argc, char **argv){

    if (argc != 4) {
    	printf("Incorrect number of arguments\n");
    	exit(1);
    }
    
    char* policy = argv[1];
    int i;
    for(i = 0; i < 3; ++i) {
    	if (strncmp(policy, policies[i], policy_len) == 0)
    		break;
    }
    
    switch (i) {
    	case 0:
    		fifo(argv[2]);
    		break;
    	case 1:
    		sjf(argv[2]);
    		break;
    	case 2:
    		rr(argv[2], atoi(argv[3]));
    		break;
    	default: 
    		printf("Incorrect policy name!\n");
    		exit(1);
    
	}
}

int fifo(char* filepath) {
	list* notCompleted;
	list* completed;
	
	init_list(&notCompleted);
	init_list(&completed);
	
	printf("Execution trace with FIFO:\n");
	FILE* fp = fopen(filepath, "r");
	if(fp == NULL) {
	 	printf("Incorrect filepath!\n");
	 	exit(1);
	}
	
	char * line = NULL;
    	size_t len = 0;
    	ssize_t read;
	int id = 0;
	while ((read = getline(&line, &len, fp)) != -1) {
       	 	add_job_to_back(notCompleted, id, atoi(line)); 
       	 	++id;
    	}
	
	job* tp = dequeue(notCompleted);
	
	int currentTime = 0;
	while(tp) {
		printf("Job %i ran for: %i\n", tp->id, tp->length);
		tp->responseTime = currentTime;
		tp->turnaroundTime = currentTime + tp->length;
		tp->waitTime = currentTime;
		
		add_job_responseTime_ascending(completed, tp);
		
		currentTime += tp->length;
		tp = dequeue(notCompleted);
	}
	printf("End of execution with FIFO.\n");
	
	printf("Begin analyzing FIFO:\n");
	tp = completed->first;
	while(tp) {
		printf("Job %i -- Response time: %i  Turnaround: %i  Wait: %i\n", tp->id, tp->responseTime, tp->turnaroundTime, tp->waitTime);
				
		tp = tp->next;
	}
	print_averages(completed);
	
	printf("End analyzing FIFO.\n");
	
    	fclose(fp);
    	free(notCompleted);
    	free(completed);
    		
}

int sjf(char* filepath) {
	list* notCompleted;
	list* completed;
	
	init_list(&notCompleted);
	init_list(&completed);
	
	printf("Execution trace with SJF:\n");
	FILE* fp = fopen(filepath, "r");
	if(fp == NULL) {
	 	printf("Incorrect filepath!\n");
	 	exit(1);
	}
	
	char * line = NULL;
    	size_t len = 0;
    	ssize_t read;
	int id = 0;
	while ((read = getline(&line, &len, fp)) != -1) {
		add_job_sjf(notCompleted, id, atoi(line));
    		++id;
    	}
	job* tp = dequeue(notCompleted);
	
	int currentTime = 0;
	while(tp) {
		printf("Job %i ran for: %i\n", tp->id, tp->length);
		tp->responseTime = currentTime;
		tp->turnaroundTime = currentTime + tp->length;
		tp->waitTime = currentTime;
		
		add_job_responseTime_ascending(completed, tp);
		
		currentTime += tp->length;
		tp = dequeue(notCompleted);
	}
	printf("End of execution with SJF.\n");
	
	printf("Begin analyzing SJF:\n");
	tp = completed->first;
	while(tp) {
		printf("Job %i -- Response time: %i  Turnaround: %i  Wait: %i\n", tp->id, tp->responseTime, tp->turnaroundTime, tp->waitTime);
				
		tp = tp->next;
	}
	print_averages(completed);
	
	printf("End analyzing SJF.\n");
	
	
    	fclose(fp);
    	free(notCompleted);
    	free(completed);
}

int rr(char* filepath, int time_slice) {
	list* notCompleted;
	list* completed;
	
	init_list(&notCompleted);
	init_list(&completed);
	
	
	printf("Execution trace with RR:\n");
	FILE* fp = fopen(filepath, "r");
	if(fp == NULL) {
	 	printf("Incorrect filepath!\n");
	 	exit(1);
	}
	
	char * line = NULL;
    	size_t len = 0;
    	ssize_t read;
	int id = 0;
	while ((read = getline(&line, &len, fp)) != -1) {
    		add_job_to_back(notCompleted, id, atoi(line));  
    		++id;
    	}
	
	int currentTime = 0;
	while (notCompleted->first) {
		job* tp = notCompleted->first;
		while(tp) {
			if(!tp->hasRun) {
				tp->responseTime = currentTime;
				tp->hasRun = 1;
			} 
				
			tp->waitTime += currentTime - tp->lastTime;
			
			
			if(tp->length > time_slice) {
				printf("Job %i ran for: %i\n", tp->id, time_slice);
				tp->length = tp->length - time_slice;
				currentTime += time_slice;
				tp->lastTime = currentTime;
				tp = tp->next;
			} else {
				printf("Job %i ran for: %i\n", tp->id, tp->length);
				tp->turnaroundTime = currentTime + tp->length; 
				job* temp = tp->next;
				currentTime += tp->length;
				tp->lastTime = currentTime;
				add_job_responseTime_ascending(completed, removeFromList(notCompleted, tp)); //add the finished packed to the completed list 
				tp = temp;
				
			}
				
		}
	}
	
	printf("End of execution with RR.\n");
    	
    	
    	printf("Begin analyzing RR:\n");
	job* tp = completed->first;
	while(tp) {
		printf("Job %i -- Response time: %i  Turnaround: %i  Wait: %i\n", tp->id, tp->responseTime, tp->turnaroundTime, tp->waitTime);
				
		tp = tp->next;
	}
	print_averages(completed);
	
	printf("End analyzing RR.\n");
	
    	
    	fclose(fp);
    	free(notCompleted);
    	free(completed);

}

