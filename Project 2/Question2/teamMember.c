#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>
#include <pthread.h>
#include <string.h>
#include <semaphore.h>

#include "util_random.h"
#include "defines.h"
#include "queue.h"


extern pkt_queue* packet_queue;

extern char* stations[];
extern char* colors[];

extern sem_t member_semaphores[NUM_TEAMS][TEAM_SIZE];
extern sem_t new_available_station[NUM_TEAMS];
extern sem_t conveyor_lock;
extern sem_t package_queue_lock;

extern sem_t print_semaphore;

extern stationStruct g_stations[NUM_STATIONS];
extern BlockedTeam blockedTeam;
extern sem_t blockedTeamSemaphore;
extern int waitingTeams;

void process(pkt_node* pkt, char* instruction,char* previousInstruction, char* memberColor, int memberIndex, int teamSemaphoreIndex);
void occupyStations(char** instructions, char* memberColor, int memberIndex, int index);
void leaveStation(char* station, char* memberColor, int memberIndex, int index, int teamSemaphoreIndex);
char* fetchNextInstruction(pkt_node* pkt);
int reserveStations(char** instructions, char* memberColor, int memberIndex, int index);
void printStations();


void* processPacket(void * arg) {
/*format the input args*/
	memberArgs* args = (memberArgs*) arg;
	char* memberColor = args->color;
	int memberIndex = args->index;
	int teamSemaphoreIndex;
	int blockedTimes = 0;
/*set up the team semaphore for the member*/

	for (teamSemaphoreIndex = 0; teamSemaphoreIndex < NUM_TEAMS; ++teamSemaphoreIndex) {
			if (strcmp(memberColor, colors[teamSemaphoreIndex]) == 0) {
				break;	//we found the right teamSemaphoreIndex
			}
	}

	sem_wait(&member_semaphores[teamSemaphoreIndex][memberIndex]);		
	
	while (1) {
	/*grab packet from queue atomically*/
		sem_wait(&package_queue_lock);
		pkt_node* pkt = dequeue(packet_queue);
		
	/*check the case where there are no more packets*/
		if(pkt == NULL) {
			
				printf("{Worker %s#%i}: No more packets, signalling others before returning\n", 
					memberColor, memberIndex);
			
			sem_post(&member_semaphores[teamSemaphoreIndex][(memberIndex + 1) % TEAM_SIZE]);
			sem_post(&package_queue_lock);
			printStations();
			return NULL;
		}
	
	/*The case where you have a packet*/
	
		printf("{Worker %s#%i}: I've picked up package #%i, with instructions: ", memberColor, memberIndex, pkt->index);
		print_instructions(pkt->instructions);
		printf("\n");
		
		sem_post(&package_queue_lock);
	/*Packet successfully grabbed*/
		
		
		
		
	/*Check if we can grab all the stations for a packet*/
		
		while(!reserveStations(pkt->instructions, memberColor, memberIndex, pkt->index)) {
			/*try and see if this team was waiting for too long*/
			sem_wait(&blockedTeamSemaphore);
			++blockedTimes;
			++waitingTeams;
			if (blockedTimes >= BLOCKED_LIMIT && blockedTeam.flag == 0) {
				if(DEBUG) {
				printf("{Worker %s#%i}: Packet #%i I am done waiting for anyone else. I demand to be the next one in line\n", 
									memberColor, memberIndex, pkt->index); 
			}
				blockedTeam.flag = 1;
				blockedTeam.teamIndex = teamSemaphoreIndex;
			}
			sem_post(&blockedTeamSemaphore);
			
			if(DEBUG) {
				printf("{Worker %s#%i}: Packet #%i is waiting for a new station to open\n", 
									memberColor, memberIndex, pkt->index); 
			}
			/*if the team was not waiting for too long, they may signal other teams as well that a free spot opened up*/
			for (int i = 1; i < NUM_TEAMS; ++i) {
				int semVal;
				sem_getvalue(&new_available_station[(teamSemaphoreIndex+i)%NUM_TEAMS], &semVal);
				if(semVal == 0 && waitingTeams < NUM_TEAMS) {
					sem_post(&new_available_station[(teamSemaphoreIndex+i)%NUM_TEAMS]);
				}
			}
			/*since they themselves may not proceed they sleep*/
			sem_wait(&new_available_station[teamSemaphoreIndex]);
			
		}
		sem_wait(&blockedTeamSemaphore);
		waitingTeams = 0;
		blockedTeam.flag = 0;
		blockedTeam.teamIndex = -1;
		sem_post(&blockedTeamSemaphore);
		
		
	/*process the packet */
		process(pkt, pkt->instructions[0], NULL, memberColor, memberIndex, teamSemaphoreIndex);
		free(pkt);
		
				
	//go back to the end of your teams queue
		if(DEBUG) {
		printf("{Worker %s#%i}: Going to the end of my queue\n", 
			memberColor, memberIndex);
		}
		
	//tell team members to start working
	//go to sleep yourself
		sem_post(&member_semaphores[teamSemaphoreIndex][(memberIndex + 1) % TEAM_SIZE]);

		sem_wait(&member_semaphores[teamSemaphoreIndex][memberIndex]);
	
	}

	return NULL;

}




//remember to add semapthores, which would be in an array with indexes corresponding to stations[] array
void process(pkt_node* pkt, char* instruction,char* previousInstruction, char* memberColor, int memberIndex, int teamSemaphoreIndex) {
 	sem_wait(&conveyor_lock);
 	printStations();
	leaveStation(previousInstruction, memberColor, memberIndex, pkt->index, teamSemaphoreIndex);	//leave the station where you are finished
	sem_post(&conveyor_lock);
	if(instruction == NULL ) {
		printf("{Worker %s#%i}: Finished processing Packet #%i, Loading it on Truck\n",
					memberColor, memberIndex, pkt->index);
		return; 
	}
	//now you are at the station where you want to be
	int processingTime = get_random_number(MIN_PROCESS_TIME, MAX_PROCESS_TIME);
	
		printf("{Worker %s#%i} [Station: %s] Processing Packet #%i, it will take %i seconds\n", 
				memberColor, memberIndex, instruction, pkt->index, processingTime);   
	
	sleep(processingTime); //simulate processing at the station
	
	if(DEBUG) {
	printf("{Worker %s#%i} [Station: %s] Finished Processing Packet #%i, sending it further\n", 
						memberColor, memberIndex, instruction, pkt->index);  
	}
	//get the next instruction that you need
	char* nextInstruction = fetchNextInstruction(pkt);
	if(DEBUG) {
		printf("{Worker %s#%i} [Station: %s] Fetched new instruction: %s\n",
						memberColor, memberIndex, instruction, nextInstruction);
	}
	
	//forward the packet to the next station
	
	process(pkt, nextInstruction, instruction, memberColor, memberIndex, teamSemaphoreIndex);
	return;
}


/*reserve all stations if all are available and return 1, otherwise return 0*/
int reserveStations(char** instructions, char* memberColor, int memberIndex, int index) {
	if(DEBUG) {
	printf("{Worker %s#%i}:  Locking conveyor belt to check available instrctions for Packet #%i\n", 
				memberColor, memberIndex, index);
	}   
	sem_wait(&conveyor_lock);
	int i = 0;
	int returnVal = 1;
	while(instructions[i]){	//as long as we can keep reading instructions
		for(int j = 0; j < MAX_NUM_INSTRUCTIONS; ++j) {		
			if (!strcmp(instructions[i], stations[j])) {	//compare the instruction to the list of stations
				if(DEBUG) {
					printf("{Worker %s#%i}:\tSeeing if station: %s is available\n",
						memberColor, memberIndex, instructions[i]);
				}
				stationStruct* stationP = &g_stations[j];		
				returnVal &= (stationP->taken == 0);		//if at least one stations is full, returns a 0
				if(DEBUG) {
				printf("{Worker %s#%i}:\tThe stations is available?: %i\n",
						memberColor, memberIndex, (stationP->taken == 0));
				}
			}
		}
	++i;
	}
	if(DEBUG) {
	printf("{Worker %s#%i}: Unlocking conveyor belt for Packet #%i\n", 
				memberColor, memberIndex, index); 
	}
	

	if (returnVal) {occupyStations(instructions,memberColor, memberIndex, index);}
	sem_post(&conveyor_lock);
	return returnVal;
}

/*marks stations in instructions as reserved atomically */
void occupyStations(char** instructions, char* memberColor, int memberIndex, int index) {
	if(DEBUG) {
		printf("{Worker %s#%i}:  Locking conveyor belt to  occupy stations for Packet #%i\n", 
				memberColor, memberIndex, index);   
	}	

	int i = 0;
	while(instructions[i]){	//as long as we can keep reading instructions
		for(int j = 0; j < MAX_NUM_INSTRUCTIONS; ++j) {
			if (!strcmp(instructions[i], stations[j])) {
				if(DEBUG) {
					printf("{Worker %s#%i}:\tAttempting to lock station: %s\n",
						memberColor, memberIndex, instructions[i]);
				}
				stationStruct* stationP = &g_stations[j];
				stationP->taken = 1;
				stationP->packetNum = index;
				stationP->teamCol = memberColor;
				stationP->teamInd = memberIndex;
				
				if(DEBUG) {
				printf("{Worker %s#%i}:\tSuccessfully locked station: %s\n",
						memberColor, memberIndex, instructions[i]);
				}
			}
		}
	++i;
	}
	
	if(DEBUG) {
		printf("{Worker %s#%i}: Unlocking conveyor belt for Packet #%i\n", 
				memberColor, memberIndex, index); 
	}
}

/*leaves the currently occupied station*/
void leaveStation(char* station, char* memberColor, int memberIndex, int index, int teamSemaphoreIndex) {
	if(DEBUG) {
	printf("{Worker %s#%i}:  Locking conveyor belt to  free station: %s for Packet #%i\n", 
				memberColor, memberIndex, station, index);   
	}
	if (station) {				   	//watch out for the non-null case
		int i;
		for(i = 0; i < MAX_NUM_INSTRUCTIONS; ++i) {
			if (!strcmp(station, stations[i])) {
				if(DEBUG) {
					printf("{Worker %s#%i}:\tUnlocking the station: %s\n",
						memberColor, memberIndex, station);
				}
				stationStruct* stationP = &g_stations[i];
				stationP->taken = 0;
				stationP->packetNum = -1;
				stationP->teamCol = "";
				stationP->teamInd = -1;
				
				sem_wait(&blockedTeamSemaphore);
				if (blockedTeam.flag) {
					sem_post(&new_available_station[blockedTeam.teamIndex]);
				} else {
					for (int i = 1; i < NUM_TEAMS; ++ i) {
						sem_post(&new_available_station[(teamSemaphoreIndex+i)%NUM_TEAMS]);
					}		
				}
				sem_post(&blockedTeamSemaphore);
				break;
			}
		}
	}
	if(DEBUG) {
		printf("{Worker %s#%i}: Unlocking conveyor belt for Packet #%i\n", 
				memberColor, memberIndex, index); 
	}
}

/*gets next instruction for packet*/
char* fetchNextInstruction(pkt_node* pkt) {
	char* nextInstruction;
	int i = 0;
	char** tmp = pkt->instructions;
	//printf("fetchNextInstruction[%i]: %s\n", i, tmp[i]);
	while(!tmp[i]) {	//loop until you encounter a non-null instruction
		//printf("fetchNextInstruction[%i]: %s\n", i, tmp[i]);
		if(++i >= MAX_NUM_INSTRUCTIONS) { //however if there are no more instructions to be performed
			 return NULL;    //the packet is ready for the truct
		}
	}
	nextInstruction = tmp[i+1];		//get the value of the next instruction
	//printf("Finished fetchNextInstruction[%i]: %s\n", i, nextInstruction);
	tmp[i] = NULL;				//clear it from the list of instructions
	return nextInstruction;
}

/*prints info about the stations stations, i.e. by which packet they are reserved */
void printStations() {
	sem_wait(&print_semaphore);		//we need to lock the conveyor belt to avoid race conditions
	
	printf("\t%s:%s#%i Packet#%i\t%s:%s#%i Packet#%i\t%s:%s#%i Packet#%i\t%s:%s#%i Packet#%i\n", 
		g_stations[0].name,g_stations[0].teamCol, g_stations[0].teamInd, g_stations[0].packetNum, 
		g_stations[1].name,g_stations[1].teamCol, g_stations[1].teamInd, g_stations[1].packetNum, 
		g_stations[2].name,g_stations[2].teamCol, g_stations[2].teamInd, g_stations[2].packetNum, 
		g_stations[3].name,g_stations[3].teamCol, g_stations[3].teamInd, g_stations[3].packetNum
		);
	
	sem_post(&print_semaphore);		//we need to lock the conveyor belt to avoid race conditions

}


