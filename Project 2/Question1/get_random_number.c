#include <stdio.h>
#include <stdlib.h>

int get_random_number(int lowerLimit, int upperLimit) {
	int range = upperLimit - lowerLimit + 1;
	return lowerLimit + (rand() % range);
}
