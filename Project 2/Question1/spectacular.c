#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>
#include <pthread.h>

#include "util_random.h"
#include "defines.h"

/*function prototypes*/
void initStage();
void initPerformers(pthread_t* performersP, performerInfo* args);

void* startPerformance(void * arg);

/*global variables*/
int performancesSoFar = 0;

Stage stage;

/*locks, condition variables*/
pthread_mutex_t stageLock;

pthread_cond_t canGoOnStageC[NUM_TYPES];


int main(int argc, char* argv[]) {
	int seed;
	read_seed("seed.txt", &seed);
	srand(seed);

	initStage();
	
	pthread_mutex_init(&stageLock, NULL);
	for (int i = 0; i<NUM_TYPES; ++i) {
		pthread_cond_init(&canGoOnStageC[i], NULL);
	}
	pthread_t performers[TOTAL_PERFORMERS];
	performerInfo args[NUMDANCERS + NUMJUGGLERS + NUMSOLOISTS];
	initPerformers(performers, args);
	
	for (int i = 0; i < TOTAL_PERFORMERS; ++i) {
		pthread_join(performers[i], NULL);
	}
	
	return 0;
}


void initStage() {
	stage.performancesSoFar = 0;
	stage.samePerformersInARow = 0;
	stage.currentTypeOfPerformance = EMPTY;
	for (int i = 0; i < STAGESIZE; ++i) {
		performerInfo* info = &(stage.spots[i]);
		info->index = -1;
		info->type = EMPTY;
	}
	return;
}

void printPerformer(Performer p) {
	switch (p) {
			case JUGGLER:
				printf("Juggler");
				break;
			case DANCER: 
				printf("Dancer");
				break;
			case SOLOIST:
				printf("Soloist");
				break;
			case EMPTY:
				printf("Empty");
				break;
			default:
				printf("Unknown - not supposed to happen\n");
				break;

	}
}

void printStage(performerInfo* spots) {
	performerInfo* info;
	for (int i = 0; i < STAGESIZE; ++i) {
		info = &spots[i];
		printf("Pos%i: ", i);
		printPerformer(info->type);
		printf("%i\t", info->index);
	}
	printf("\n");
}

void initPerformers(pthread_t* performersP, performerInfo* args) {

	int i;
	for (i = 0; i <  NUMDANCERS + NUMJUGGLERS + NUMSOLOISTS; ++i) {
		performerInfo* arg = &args[i];
		arg->index = i;
		if (i < NUMDANCERS) {
			arg->type = DANCER;
		}else if(i >= NUMDANCERS && i < NUMDANCERS + NUMJUGGLERS) {
			arg->type = JUGGLER;
		}
		else {
			arg->type = SOLOIST;
		}
			
		pthread_create(&performersP[i], NULL, startPerformance, (void*) &args[i]);
	}
	return;
}




