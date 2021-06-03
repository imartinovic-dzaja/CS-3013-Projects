#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>

#include "queue.h"
#include "defines.h"


void init_queue(pkt_queue** q) {
    *q = (pkt_queue*) malloc(sizeof(pkt_queue));
    memset(*q, 0, sizeof(pkt_queue)); /*sets all pointers to null and size to 0*/
}

void enqueue(pkt_queue* q, pkt_node* pkt) {
    if (!(q->first)) {      /*if the first node is null then alter it*/
        q->first = pkt;
        q->last = q->first;
    }else{
        q->last->next = pkt;
        q->last = pkt;                                       /*make it the new last node*/
    }
}

/*returns the address of the last element and removes it from the queue*/
/*it is responsibility of the caller to free the allocated space*/
struct pkt_node* dequeue(pkt_queue *q) {
    if (!(q->first)) {
        return 0;
    } else if(!(q->first->next)) {
        struct pkt_node* originalFirst = q->first;
        q->first = 0;
        return originalFirst;
    }
    else {
        struct pkt_node* originalFirst = q->first;
        q->first = originalFirst->next;
        originalFirst->next = NULL; 		//once the node is removed from the queue it has no next
        return originalFirst;
    }
}

void print_queue(pkt_queue *q) {
    if(!(q->first)) {
        printf("\n");
    }else {
        struct pkt_node* tp = q->first;
        int i = 1;
        while(tp->next) {      /*keep stepping through nodes till you get to the one that has no next*/
            printf("Packet %i: ", i++);
            print_instructions(tp->instructions);
            printf("\n");
            tp = tp -> next;
        }
        printf("Packet %i: ", i++);
        print_instructions(tp->instructions);
        printf("\n");
     }
}

void print_instructions(char** instructions) {
	for (int i = 0; i < 4; ++i) {
		if(instructions[i]) 		//if the instruction is non-null, print it
			printf("%s ", instructions[i]);
	}
}

