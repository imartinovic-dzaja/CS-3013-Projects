#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "include.h"

/*initialize a list*/
void init_list(list** l) {
	*l =  (list*) malloc(sizeof(struct list));
    	memset(*l, 0, sizeof(l)); /*sets all pointers to null */
}

/*allocate space for a new job node and add it to the front of the list*/
void add_job_to_front(list* l, int id, int length) {
        job* tp = (job*) malloc(sizeof(job));    	/*allocate space for new node*/
	tp->next = l->first;		
    	l->first = tp;                                       /*make it the new first node*/
    	if (tp->next ==NULL) {      			/*if we inserted our first node */
        	l->last = l->first; 			/* set the last and first pointer equal to each other*/
 	}
    	tp->id = id;
        tp->length = length;
}

/*allocate space for a new job and add it to the list according to sjf*/
void add_job_sjf(list* l, int id, int length) {
	
	job* ltp = l->first;
	if (ltp == NULL || ltp->length > length) {
		add_job_to_front(l, id, length);
	} else {
		job* insertedJob = (job*) malloc(sizeof(job));    	/*allocate space for new node*/
		insertedJob->id = id;
        	insertedJob->length = length;
        	while(ltp->next != NULL && ltp->next->length <= length) {
        		ltp = ltp->next;
        	}
        	job* tp = ltp->next;
        	ltp->next = insertedJob;
        	insertedJob->next = tp;
	}
}

/*allocate space for a new job and add it to the back of the list*/
void add_job_to_back(list* l, int id, int length) {
        job* tp = (job*) malloc(sizeof(job));    	/*allocate space for new node*/
        memset(tp, 0, sizeof(job));
	tp->id = id;
        tp->length = length;
        tp->next = NULL;
        
    	if (l->last == NULL) {      			/*if we inserted our first node */
        	l->first = tp;
        	l->last = tp;
        	return;
 	}
 	l->last->next = tp;
 	l->last = tp;
 	
}

/*take a finished job and add it to the list of finished job in order of ascending responseTimes */
void add_job_responseTime_ascending(list* l, job* j) {
	job* prev = l->first;
	
	if(prev) {
		if(j->responseTime < l->first->responseTime) {
			j->next = l->first;
			l->first = j;
			return;
		} else {
			job* temp = prev->next;
			while(temp) {
				if (j->responseTime < temp->responseTime) {
					prev->next = j;
					j->next = temp;
					return;
				}
				prev = temp;
				temp = temp->next;
			}
			prev->next = j;
			l->last = j;
			j->next = NULL;
		}
	}
	
	else {
		l->first = j;
		l->last = j;
		j->next = NULL;
	}
}

/*dequeue the first job on the list*/
job* dequeue(list *l) {
 	job* returnNode = l->first;
 	if(returnNode != NULL) {
 		l->first = returnNode->next;
 	}
 	//returnNode->next = NULL;
 	return returnNode; 

}

/*remove the specified job from the list of jobs*/
job* removeFromList(list* l, job* j) {
	/*edge case*/
	if(l->first == j) {
		l->first = j->next;
		if(j->next == NULL) 
			l->last = NULL;
		return j;
	}
	job* tp = l->first;
	while(tp != NULL) {
		if(tp->next == j) {
			tp->next = j->next;
			j->next = NULL;
			return j;
		}
		tp = tp->next;
	}
	return NULL;	//j not found
}

/*print the averages*/
void print_averages(list *l) {
	int numJobs = 0;
	double totalResponseTime = 0;
	double totalTurnaroundTime = 0;
	double totalWaitTime = 0;
	job* tp = l->first;
	while(tp) {
		totalResponseTime += tp->responseTime;
		totalTurnaroundTime += tp->turnaroundTime;
		totalWaitTime += tp->waitTime;
		++numJobs;
		
		tp = tp->next;
	}
	
	printf("Average -- Response: %.2f  Turnaround %.2f  Wait %.2f\n", totalResponseTime/numJobs, totalTurnaroundTime/numJobs, totalWaitTime/numJobs);
	

}



