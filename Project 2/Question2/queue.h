#ifndef QUEUE_H_INCLUDED
#define QUEUE_H_INCLUDED

struct pkt_node;

typedef struct pkt_node{
    char** instructions;
    int index;
    struct pkt_node* next;  
} pkt_node;

typedef struct pkt_queue {
    pkt_node* first;
    pkt_node* last;
} pkt_queue;

void init_queue(pkt_queue** q);
void enqueue(pkt_queue* q, pkt_node* pkt);
struct pkt_node* dequeue(pkt_queue *q);
void print_queue(pkt_queue *q);
void print_instructions(char** instructions);





#endif // QUEUE_H_INCLUDED
