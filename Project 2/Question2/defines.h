#include "queue.h"

#ifndef DEFINES_H
#define DEFINES_H

#define NUM_TEAMS 4
#define NUM_STATIONS 4
#define TEAM_SIZE 10
#define MAX_NUM_INSTRUCTIONS 4

#define MIN_PROCESS_TIME 1
#define MAX_PROCESS_TIME 5

#define BLOCKED_LIMIT 10
#define NUM_PACKETS 50

#define DEBUG 0

typedef struct memberArgs {
	char* color;
	int index;
} memberArgs;

typedef struct station{
	int taken;
	int packetNum;
	char* name;
	char* teamCol;
	int teamInd;

} stationStruct;

typedef struct BlockedTeam {
	int flag;
	int teamIndex;
} BlockedTeam;

#endif /* DEFINES_H */
