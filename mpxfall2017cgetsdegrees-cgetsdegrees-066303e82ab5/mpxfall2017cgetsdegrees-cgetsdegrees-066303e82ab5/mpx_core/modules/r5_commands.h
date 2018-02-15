//header file for r5
#ifndef R5_COMMANDS_H
#define R5_COMMANDS_H
#include <system.h>
#include "r2_commands.h"

typedef struct CMCB{
	int type; //0 for free, 1 for allocated
	char processAllocated[20];
	u32int sizeBlock; 
	u32int beginAddress;
	struct CMCB* next;
	struct CMCB* prev;
}CMCB;
typedef struct LMCB{
	int type;
	u32int sizeBlock;
}LMCB;

int initializeHeap(int size);

u32int allocMem(u32int size);

int freeMem(void *ptr);

void showA();

void showF();

int isEmpty();

int freeIn(CMCB* toInsert);

int aIn(CMCB* toInsert);

void heapHelp();

void* allocHelp();

void freeHelp();

void merge();

#endif
