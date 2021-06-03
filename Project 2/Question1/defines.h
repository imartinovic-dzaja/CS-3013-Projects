#ifndef DEFINES_H
#define DEFINES_H

#define STAGESIZE 4
#define NUMDANCERS 15
#define NUMJUGGLERS 8
#define NUMSOLOISTS 2
#define TOTAL_PERFORMERS NUMDANCERS+NUMJUGGLERS+NUMSOLOISTS
#define NUM_TYPES 3

#define SHORTEST_PERFORMANCE 1
#define LONGEST_PERFORMANCE 4
#define TOTAL_PERFORMANCES 75

#define MAXIMUM_CONSECUTIVE_PERFORMANCES 5


#define DEBUG 0

typedef enum {
	JUGGLER = 0,
	DANCER = 1,
	SOLOIST = 2,
	EMPTY
} Performer;

typedef struct {
	Performer type;
	int index;
} performerInfo;

typedef struct Stage{
	int performancesSoFar;
	Performer currentTypeOfPerformance;
	int samePerformersInARow;
	performerInfo spots[STAGESIZE];
} Stage;

#endif /* DEFINES_H */
