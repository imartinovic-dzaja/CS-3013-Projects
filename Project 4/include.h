#ifndef INCLUDE_H
#define INCLUDE_H

typedef struct job {
	int id;
	int length;
	int responseTime;
	int turnaroundTime;
	int waitTime;
	int hasRun;
	int lastTime;
	struct job* next;

} job;

typedef struct list {
	job* first;
	job* last;
} list;

void init_list(list** l);
void add_job_to_front(list* l, int id, int length);
void add_job_sjf(list* l, int id, int length);
void add_job_to_back(list* l, int id, int length);
void add_job_responseTime_ascending(list* l, job* j);
job* dequeue(list *l);
job* removeFromList(list* l, job* j);
void print_averages(list *l);

#endif /* INCLUDE_H */
