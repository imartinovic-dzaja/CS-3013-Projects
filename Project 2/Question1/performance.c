#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>
#include <pthread.h>

#include "util_random.h"
#include "defines.h"

/*function prototypes*/
void printPerformer(Performer p);
void printStage(performerInfo* stageP);
void getStageInfo(performerInfo* stageP, int* result);
void makeDecision(Performer performerType, int performerIndex, int hasFreeSpot, int isEmpty, int* canGo);

/*global vars*/

extern Stage stage;


/*mutexes and condition variables*/
extern pthread_mutex_t stageLock;
extern pthread_cond_t canGoOnStageC[NUM_TYPES];

void* startPerformance(void * arg) {
	performerInfo* args = (performerInfo*) arg;
	int performerIndex = args->index;
	Performer performerType = args->type;
	
	int stageInfo[3];
	int hasFreeSpot;
	int isEmpty;
	int freeSpotIndex;
	while(1) {

		pthread_mutex_lock(&stageLock);
		int canGo = 0;

		while(canGo == 0) {
			//check if we've exceeded the maximum number of performances
			if(stage.performancesSoFar >= TOTAL_PERFORMANCES) {
				if (DEBUG) {
				printf("[Index: %i, Type:", performerIndex);
				printPerformer(performerType);
				printf("]: That was: %i performances, now exiting\n", stage.performancesSoFar);
				}
			
				//wake up all the other threads and let them terminate
				pthread_cond_broadcast(&canGoOnStageC[1]); 
				pthread_cond_broadcast(&canGoOnStageC[1]); 
				pthread_cond_broadcast(&canGoOnStageC[2]); 
				pthread_mutex_unlock(&stageLock);
				return NULL;
			}
		

			getStageInfo(stage.spots, stageInfo);
			hasFreeSpot = stageInfo[0];
			isEmpty = stageInfo[1];
			freeSpotIndex = stageInfo[2];
			
			makeDecision(performerType,performerIndex, hasFreeSpot, isEmpty, &canGo);
		}
	
		//increment the total number of performances and occupy the spot on stage
		++(stage.performancesSoFar);
		performerInfo* spot = &(stage.spots[freeSpotIndex]);
		spot->type = performerType;
		spot->index = performerIndex;
		int performanceLength = get_random_number(SHORTEST_PERFORMANCE, LONGEST_PERFORMANCE);
		if (1) {
		printf("[Index: %i, Type:", performerIndex);
		printPerformer(performerType);
		printf("]: This will be performance #%i\n", stage.performancesSoFar);
		
		printf("[Index: %i, Type:", performerIndex);
		printPerformer(performerType);
		printf("]: I will now perform for %i seconds\n", performanceLength);
		}
		
		printf("[Index: %i, Type:", performerIndex);
		printPerformer(performerType);
		printf("]:\t");
		printStage(stage.spots);
		
		pthread_mutex_unlock(&stageLock);
		sleep(performanceLength);	//simulate performance
		
		pthread_mutex_lock(&stageLock);
		
		if (1) {
		printf("[Index: %i, Type:", performerIndex);
		printPerformer(performerType);
		printf("]: I am done performing\n");
		}
		
		spot->type = EMPTY;
		spot->index = -1;
		if (DEBUG) {
		printf("[Index: %i, Type:", performerIndex);
		printPerformer(performerType);
		printf("]: I am signalling to my group\n");
		}
		
		pthread_cond_signal(&canGoOnStageC[performerType]); //wake up other threads
		
		printf("[Index: %i, Type:", performerIndex);
		printPerformer(performerType);
		printf("]:\t");
		printStage(stage.spots);
		
		getStageInfo(stage.spots, stageInfo);
		isEmpty = stageInfo[1];
		if (isEmpty) {
			if (DEBUG) {
			printf("[Index: %i, Type:", performerIndex);
			printPerformer(performerType);
			printf("]: The stage is empty now, I will broadcast\n");
			}
			pthread_cond_broadcast(&canGoOnStageC[(performerType+1)%NUM_TYPES]); //wake up other threads
			pthread_cond_broadcast(&canGoOnStageC[(performerType+2)%NUM_TYPES]); //wake up other threads
		}
		pthread_cond_wait(&canGoOnStageC[performerType], &stageLock);				
		pthread_mutex_unlock(&stageLock);
		
	}
	return NULL;
}

/*checks if the stage has a free spot (index 0), if the stage is empty (index 1), and the index of a free spot (index 2)*/
void getStageInfo(performerInfo* stageP, int* result) {
	int hasFreeSpot = 0;
	int isEmpty = 1;
	int freeSpotIndex = -1;
	
	for (int i = 0; i < STAGESIZE; ++i) {
		performerInfo* info = &stageP[i];
		isEmpty &= (info->type == EMPTY);
		hasFreeSpot |= (info->type == EMPTY);
		if (info->type == EMPTY) {
			freeSpotIndex = i;
		}
	}
	result[0] = hasFreeSpot;
	result[1] = isEmpty;
	result[2] = freeSpotIndex;
}

void makeDecision(Performer performerType, int performerIndex, int hasFreeSpot, int isEmpty, int* canGo) {
	if (DEBUG){
		printf("[Index: %i, Type:", performerIndex);
		printPerformer(performerType);
		printf("]:\t");
		printStage(stage.spots);	
		printf("[Index: %i, Type:", performerIndex);
		printPerformer(performerType);
	}
	
	if (hasFreeSpot) {
		if (isEmpty) { 				
			if (performerType != stage.currentTypeOfPerformance) {
				if (DEBUG) {printf("]: The stage is empty, I'll go on-stage\n");}
				stage.samePerformersInARow = 1;				
				stage.currentTypeOfPerformance = performerType;
				*canGo = 1;
			} else {
				if (DEBUG) {printf("]: The stage is empty, but we've been hogging the stage (hogCount: %i), I'll go sleep\n", stage.samePerformersInARow);}												
				pthread_cond_wait(&canGoOnStageC[performerType], &stageLock);
			}
		} else {
			if (performerType != stage.currentTypeOfPerformance) {
				if (DEBUG) {printf("]: There are free spots on the stage, but other types of performers are there, I'll go sleep\n");}		
				pthread_cond_wait(&canGoOnStageC[performerType], &stageLock);
			} else {
				if (performerType == SOLOIST) {
					if (DEBUG) {printf("]: There are free spots on the stage, but a soloist is performing, I'll go sleep\n");}
					pthread_cond_wait(&canGoOnStageC[performerType], &stageLock);
				} else if (stage.samePerformersInARow < MAXIMUM_CONSECUTIVE_PERFORMANCES) {
					if (DEBUG) {printf("]: There are free spots on the stage, my type of performers are there, and we haven't been hogging the stage (hogCount: %i), I'll go on-stage\n", stage.samePerformersInARow);}	
					++(stage.samePerformersInARow);
					*canGo = 1;	
				} else {
					if (DEBUG) {printf("]: There are free spots on the stage, my type of performers are there, but we have been hogging the stage (hogCount: %i), I'll go to sleep\n", stage.samePerformersInARow);}	
					pthread_cond_wait(&canGoOnStageC[performerType], &stageLock);
				}
			}
		}
	}
	else {
		if (DEBUG) {printf("]: The stage is full, I'll go sleep\n");}
		pthread_cond_wait(&canGoOnStageC[performerType], &stageLock);
	}


}

