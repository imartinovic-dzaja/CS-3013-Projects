#include <stdlib.h>
#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include <errno.h>
#include "goatmalloc.h"

#define DEBUG 0
#define BUFSIZE 64

int statusno = ERR_UNINITIALIZED;
static node_t* head = NULL;
static size_t structSize = sizeof(node_t);
static intptr_t totalAllocatedSize = 0;			//when destroying, we want to know by how much space to munmap


FILE *fp = NULL;
//when we initialize we create the linkelist for available spots on the heap

int init(size_t size) {
	while(fp == NULL) {
		fp = fopen("output_ref.txt","w");
	}
		
	fprintf(fp, "Initializing the arena\n");
	fprintf(fp, "...requested size %i bytes\n",(int) size);
		
	size_t pageSize = getpagesize();
	size_t allocatedSize;
	if((int) size < 0) {
		if(DEBUG) {
			printf("init(): Supplied a negative value for size\n");
		}
		return ERR_BAD_ARGUMENTS;
	}else {
		fprintf(fp, "...pagesize is %u bytes\n", (unsigned int) pageSize);
		fprintf(fp, "...adjusting size with page boundaries\n");
		if (DEBUG) { 
			fprintf(fp, "Size is : %u\n", (unsigned int) size);
		}
		allocatedSize = ((size_t) ceil(size/(double)pageSize))*pageSize;
		fprintf(fp, "...adjusted size is %u bytes\n", (unsigned int)allocatedSize);
		
		fprintf(fp, "...mapping arena with mmap()\n");
		int fd=open("/dev/zero",O_RDWR);
		void* mmapReturn = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
		while (mmapReturn == (void*) -1) {
			fprintf(stderr, "Value of errno: %d\n", errno);
  			perror("Error printed by perror");
     			fprintf(stderr, "Error calling sbrk while allocating: %s\n", strerror( errno ));
			
			mmapReturn = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
		}
		close(fd);
		
		
		totalAllocatedSize = allocatedSize;
		head = (node_t*) mmapReturn; 
		fprintf(fp, "...arena starts at %p\n", head);
		fprintf(fp, "...arena ends at %p\n", ((char*) head) + allocatedSize);
		fprintf(fp, "...initializing header for initial free chunk\n");
		fprintf(fp, "...header size is %u bytes\n", (unsigned int) structSize); 
		head->size = allocatedSize - structSize;		
		head->is_free = 1;
		head->fwd = NULL;
		head->bwd = NULL;
		statusno = 0;
		if(DEBUG) {
			fprintf(fp,"init(): Successfully initialized\n");
		}
		return allocatedSize;
	}
}

int destroy() {
	fprintf(fp, "Destroying the arena\n");
	if (statusno == ERR_UNINITIALIZED) {
		if (DEBUG) {
			fprintf(fp, "destroy(): It is uninitialized\n");
		}
		return ERR_UNINITIALIZED;
	}
	
	fprintf(fp, "...unmapping arena with munmap()\n");
	int munmapReturn = munmap(head, totalAllocatedSize);
	while (munmapReturn == -1) {
		fprintf(stderr, "Value of errno: %d\n", errno);
  		perror("Error printed by perror");
     		fprintf(stderr, "Error calling sbrk while de-allocating: %s\n", strerror( errno ));
		munmapReturn = munmap(head, totalAllocatedSize);
	}
	totalAllocatedSize = (intptr_t) 0;
	head = NULL;
	statusno = ERR_UNINITIALIZED;
	return 0;
}


void wfree(void *ptr) {
	fprintf(fp, "Freeing memory\n");
	fprintf(fp, "...supplied pointer at %p\n", ptr);
	if (DEBUG) {
		fprintf(fp, "wfree(): attempting to free: %p:\n", ptr);
	}
	fprintf(fp, "...being careful with my pointer arithmetic and void pointer casting\n");
	node_t* node = ((node_t*) ptr) - 1;
	fprintf(fp, "...accessing chung header at %p\n", node);
	fprintf(fp, "...chunk of size %u\n", (unsigned int) node->size);
	node->is_free = 1;
	
	/*3 additional cases to consider*/
	node_t* fwdNode = node->fwd;
	node_t* bwdNode = node->bwd;
	
	fprintf(fp, "...checking if coalescing is needed\n");
	/*1) Both the fwd and bwd node are also free*/
	if (fwdNode != NULL && bwdNode != NULL && fwdNode->is_free == 1 && bwdNode->is_free == 1) { 
		fprintf(fp, "...coalescing is needed: fwd and bwd are free\n");
		bwdNode->fwd =  fwdNode->fwd;
		node_t* temp = fwdNode->fwd;
		if (temp != NULL) {
			temp->bwd = bwdNode;
		}
		bwdNode->size = bwdNode->size + node->size + fwdNode->size + 2 * structSize;
	}
	/*2) Only the bwd node is free*/ 
	else if (bwdNode != NULL && bwdNode->is_free == 1) {
		fprintf(fp, "...coalescing is needed: bwd is free\n");
		bwdNode->fwd =  node->fwd;
		node_t* temp = node->fwd;
		if (temp != NULL) {
			temp->bwd = bwdNode;
		}
		bwdNode->size = bwdNode->size + node->size + structSize;
		
	}
	/*3) Only the fwd node is free*/
	else if (fwdNode != NULL && fwdNode->is_free == 1) {
		fprintf(fp, "...coalescing is needed: bwd is free\n");
		node->fwd =  fwdNode->fwd;
		node_t* temp = fwdNode->fwd;
		if (temp != NULL) {
			temp->bwd = node;
		}
		node->size = node->size + fwdNode->size + structSize;
	} else {
		fprintf(fp, "...coalescing is not needed\n");
	}
		
}

void* walloc(size_t size) {
	fprintf(fp, "Allocating memory\n");
	
	if (statusno == ERR_UNINITIALIZED) {
		if (DEBUG) {
			fprintf (fp, "walloc(): Uninitialized!\n");
		}
		fprintf(fp, "...arena is uninitialized\n");
		return NULL;
	}
		
	
	node_t* temp = head;

	
	
	/*find empty chunk big enough*/
	fprintf(fp, "...looking for free chunk >= %u\n", (unsigned int) size);
	while(temp->size < size || temp->is_free == 0) {
			
		if (temp->fwd != NULL) {
			temp = temp->fwd;
		} else {
			statusno = ERR_OUT_OF_MEMORY;
			fprintf(fp, "...out of memory\n");
			return NULL;
		}
	}
	/*empty chunk found, now add/re-arrange nodes in list*/
	fprintf(fp, "...found free chunk of size %u with header at %p\n", (unsigned int) temp->size, temp);
	fprintf(fp, "...free chunk->fwd currently points %p\n", temp->fwd);
	fprintf(fp, "...free chunk->bwd currently points %p\n", temp->bwd);
	/*	make copies of old information*/
	node_t* oldFwd = temp->fwd;
	size_t oldSize = temp->size;
	
	/*first set the original free node*/
		fprintf(fp, "...checking if splitting is required\n");
	if ((int) (temp->size - size) > (int)structSize) { //case where we can insert a new node 
		fprintf(fp, "...splitting is required\n");
		char* tempNextNode = (char*) temp;			//make it a char* to make pointer arithmetic simpler
		tempNextNode = tempNextNode + structSize + size;		//set it to the correct memory location	
		/*set values in temp node*/
		temp->fwd = (node_t*) tempNextNode;
		temp->size = size;			
		
		/*set values in next node*/
		node_t* newFreeNode = temp->fwd;
		newFreeNode->size = oldSize - size - structSize;
		newFreeNode->is_free = 1;	
		newFreeNode->bwd = temp;
		newFreeNode->fwd = oldFwd;

	} else {fprintf(fp, "...splitting is not required\n");}
	
	/*set the old node as taken*/
	fprintf(fp, "...updating chunk header at %p\n", temp);
	temp->is_free = 0;
	
	fprintf(fp, "...being careful with my pointer arithmetic and void pointer casting\n");
	fprintf(fp, "...allocation starts at %p\n", (temp+1));
	return (void*) (temp + 1); 
}
