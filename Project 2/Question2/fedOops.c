#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>

#include "util_random.h"
#include "defines.h"
#include "queue.h"

int isAlreadyThere(char** instructions, char* instruction);
struct pkt_node* generatePacket() ;
void* processPacket(void * arg); 



char* stations[] = {
	"Scale",
	"Barcode",
	"X-ray",
	"Shaker"
};

char* colors[] = {
	"Red",
	"Blue",
	"Green",
	"Yellow"

};

pkt_queue* packet_queue;
stationStruct g_stations[NUM_STATIONS];
BlockedTeam blockedTeam;
int waitingTeams = 0;

sem_t member_semaphores[NUM_TEAMS][TEAM_SIZE];
sem_t new_available_station[NUM_TEAMS];
sem_t conveyor_lock;
sem_t package_queue_lock;
sem_t blockedTeamSemaphore;
sem_t print_semaphore;



int main(int argc, char* argv[]) {
	int seed;
	read_seed("seed.txt", &seed);
	srand(seed);
	
	printf("Initializing queue\n");
	init_queue(&packet_queue);
	printf("Generating packets\n");
	for (int i = 0; i<NUM_PACKETS; ++i) {
		pkt_node* tmp = generatePacket(i);
		enqueue(packet_queue,tmp);

	}
	printf("Printing queue\n");
	print_queue(packet_queue);
	
	/*Initially we don't have a team that is blocked*/
	blockedTeam.flag = 0;
	blockedTeam.teamIndex = -1;
	
	/*Initialize the semaphores*/
	sem_init(&conveyor_lock, 0, 1);
	sem_init(&package_queue_lock, 0, 1);
	sem_init(&print_semaphore, 0, 1);
	sem_init(&blockedTeamSemaphore, 0, 1);
	for (int i = 0; i < NUM_TEAMS; ++i) {
		for (int j = 0; j < TEAM_SIZE; ++j) {
			sem_init(&member_semaphores[i][j], 0, 0);
		}
		sem_post(&member_semaphores[i][0]);
		sem_init(&new_available_station[i], 0, 0);
	}
	

		
	/*All stations are initially empty*/
	for (int i = 0; i < NUM_STATIONS; ++i) {
		stationStruct* stationP = &g_stations[i];
		stationP->taken = 0;
		stationP->packetNum = -1;
		stationP->teamCol = "";
		stationP->teamInd = -1;
		stationP->name = stations[i];
	}
	


	/*Initialize the team member threads*/
	pthread_t redTeamMembers[NUM_TEAMS][TEAM_SIZE]; 
	memberArgs memberArgs[NUM_TEAMS][TEAM_SIZE]; 
	
	for (int i = 0; i < NUM_TEAMS; ++i) {
		for(int j = 0; j < TEAM_SIZE; ++j) {
			memberArgs[i][j].color = colors[i];
			memberArgs[i][j].index = j;
			pthread_create(&redTeamMembers[i][j], NULL, processPacket, (void*) &memberArgs[i][j]);
		}
	
	}	
	
	//wait for the team members to finish
	for (int i = 0; i < NUM_TEAMS; ++i) {
		for(int j = 0; j < TEAM_SIZE; ++j) {
			pthread_join(redTeamMembers[i][j], 	NULL);
		}
	}
	return 0;
}

struct pkt_node* generatePacket(int index) {
	pkt_node* packet = NULL;
	do{packet = (pkt_node*) malloc(sizeof(pkt_node));	//malloc space for the pkt_node
	} while (packet == NULL);				//make sure it doesn't fail
	
	char** instructions = (char**) malloc(sizeof("Barcode")*4);	//malloc space for the instructions
	memset(instructions, sizeof("Barcode")*4, 0);			//zero-out the instructions
	
	int numOfInstructions = get_random_number(1, MAX_NUM_INSTRUCTIONS); 	//get a random number of instructions (from 1 to 4)
	for (int i = 0; i < numOfInstructions; ++i) {			   	//generate that number of new instructions
		char* newInstruction;
		int instructionNumber;
		do{
			instructionNumber = get_random_number(0,MAX_NUM_INSTRUCTIONS-1);
			newInstruction = stations[instructionNumber];		//choose an instruction from the stations global var
		}while (isAlreadyThere(instructions, newInstruction));	//make sure to not have duplicate instructions
		instructions[i] = newInstruction;
	}
	packet->instructions = instructions;					//set the instructions pointer 
	packet->index = index;							//set the index
	packet->next = NULL;							//packet is not in a queue, hence he has no next
	return packet;
}

//return 1 if an instruction is already within a list of instructions, 0 otherwise
int isAlreadyThere(char** instructions, char* instruction) {
	for (int i = 0; instructions[i]; ++i) {
		if(!strcmp(instructions[i], instruction))
			return 1; 
	} 
	return 0;
}
