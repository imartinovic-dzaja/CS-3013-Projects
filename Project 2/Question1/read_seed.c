#include <stdlib.h>
#include <stdio.h>

void read_seed(char* filepath, int* seed) {
    FILE* fp;
    if ((fp = fopen(filepath, "r")) == NULL) {
        printf("Couldn't open file: %s\n", filepath);
        exit(1);
    }

    int maxNumDigits = 20;
    char stringNumber[maxNumDigits];
    if (fgets(stringNumber, maxNumDigits, fp) == NULL) {
        printf("Couldn't read line from file: %s\n", filepath);
        exit(1);
    }

    *seed = atoi(stringNumber);
    printf("Seed is: %s\n", stringNumber);
    return;
}
