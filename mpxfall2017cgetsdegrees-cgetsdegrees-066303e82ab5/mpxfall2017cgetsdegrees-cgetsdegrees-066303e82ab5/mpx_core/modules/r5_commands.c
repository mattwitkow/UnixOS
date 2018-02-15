#include <core/serial.h>
#include <core/io.h>
#include <system.h>
#include <string.h>
#include <mem/heap.h>
#include "mpx_supt.h"
#include "r4init.h"
#include "r1_input.h"
#include "r2_commands.h"
#include "r3_commands.h"
#include "r5_commands.h"
/*
You will then put a CMCB at the top of the heap
and a LMCB at the bottom of the heap, both of type
free
This is also where you will want to initialize your
free and allocated lists
*/
u32int startHeapAddress;//start of memory global pointer
u32int* ptr;
CMCB* freeHead = NULL;
CMCB* allocHead = NULL;

int initializeHeap(int size){

	startHeapAddress = kmalloc(sizeof(CMCB) + sizeof(LMCB) + size);
	ptr = &startHeapAddress;
	if (!ptr)
	{
		char* message ="ptr after using kmalloc was null";
		int mLength = strlen(message);
		sys_req(WRITE, DEFAULT_DEVICE, message, &mLength);
        	return 0;
	}
	u32int jch = (u32int)size;

	CMCB* cmcb = (CMCB*)(startHeapAddress);
	cmcb->sizeBlock = jch;
	cmcb->type = 0;
	cmcb->beginAddress = (u32int)(startHeapAddress+(u32int)(sizeof(CMCB)));
	cmcb->next = NULL;
	cmcb->prev = NULL;
	freeHead = cmcb;

	LMCB* lmcb = (LMCB*)(startHeapAddress + jch + (u32int)(sizeof(CMCB)));
	lmcb->sizeBlock = jch;
	lmcb->type = 0;

return jch;

}

int freeMem(void *pptr){
	u32int addr = (u32int) pptr;
	CMCB* trav = allocHead;
	CMCB* temp;
	int flag = 1;

	while(trav != NULL){
		if(trav->beginAddress == addr){
			temp = trav;
			flag = 0;
		}
		trav = trav->next;
	}
	if(flag){
		char* message = "no block found with that address \n";
        	int mLength = strlen(message);
        	sys_req(WRITE, DEFAULT_DEVICE, message, &mLength);
		return 0;
	}
	if(temp->type == 0){
		char* message = "that cant be freed\n";
        	int mLength = strlen(message);
        	sys_req(WRITE, DEFAULT_DEVICE, message, &mLength);
		return 0;
	}
	else{
		//LMCB* tempL = (LMCB*)temp + temp->sizeBlock + sizeof(CMCB);;
		CMCB* pre = temp->prev;
		CMCB* nex = temp->next;
		if(nex){
			nex->prev = pre;
		}
		if(pre){
			pre->next = nex;
		}
		if(temp == allocHead){
			allocHead = nex;
			allocHead->prev = NULL;
		}
		if(freeIn(temp) == 0){
			char* message = "failed to insert into free list\n";
        		int mLength = strlen(message);
        		sys_req(WRITE, DEFAULT_DEVICE, message, &mLength);
			return 0;
		}
	}
	//REMOVE THESE PRINTS AFTER R5 part 1
	char* message3 = "Starting address of freed block is start of heap + ";
        int mLength3 = strlen(message3);
        sys_req(WRITE, DEFAULT_DEVICE, message3, &mLength3);
	int hello = (int)(temp->beginAddress - *ptr);
	char* bleh = "";
	itoa(hello, bleh, 10);
	int blehl = strlen(bleh);
	sys_req(WRITE, DEFAULT_DEVICE, bleh, &blehl);
	return 1; //success
}

int freeIn(CMCB* toInsert){
	CMCB* fiterate = freeHead;
	int flag1 = 0;
	int flag2 = 1;
	if(!fiterate){
		char* message = "null free list\n";
        	int mLength = strlen(message);
        	sys_req(WRITE, DEFAULT_DEVICE, message, &mLength);
		return 0;
	}
	
	if(toInsert->beginAddress < fiterate->beginAddress){ //if toInsert needs to be new head

		freeHead = toInsert;
		toInsert->next = fiterate;
		toInsert->prev = NULL;
		fiterate->prev = toInsert;
		flag1 = 1;
	}
	else if(toInsert->beginAddress == fiterate->beginAddress){//to insert is the head (first alloc)

		toInsert->next = fiterate->next;
		toInsert->prev = NULL;
		toInsert->next->prev = toInsert;
		freeHead = toInsert;
		flag1 = 1;
	}
		
	else{
		CMCB* fp;
		while(fiterate != NULL){
			if(toInsert->beginAddress < fiterate->beginAddress){

				fp->next = toInsert;
				fiterate->prev = toInsert;
				toInsert->prev = fp;
				toInsert->next = fiterate;
				flag1 = 1;
				flag2 = 0;
				break;
			}
			fp = fiterate;
			fiterate = fiterate->next;
			
		}
		if(flag2){

			toInsert->next = NULL;
			toInsert->prev = fp;
			fp->next = toInsert;
			flag1 = 1;
		}
			
	}
	if(flag1 == 0){
		char* message = "could not insert into free list\n";
        	int mLength = strlen(message);
        	sys_req(WRITE, DEFAULT_DEVICE, message, &mLength);
		return 0;
	}
	//FROMER HERE ON, IT IS LOOKING TO MERGE ADJECENT FREE BLOCKS
	merge();
	toInsert->type = 0;
	LMCB* til = (LMCB*)((u32int)toInsert + toInsert->sizeBlock + (u32int)(sizeof(CMCB)));
	til->type = 0;
	return 1;
}

u32int allocMem(u32int size){
	
	// total size of memory allocated
	int allocSize = (int)size + sizeof(CMCB) + sizeof(LMCB);
	//now find a block big enough to hold allocSize
	CMCB* cmcb = freeHead;
	while ((cmcb -> sizeBlock) < (u32int)allocSize) {
		if(!cmcb -> next){
			char* message = "no block large enough to allocate memory\n"; // end of list
			int mLength = strlen(message);
			sys_req(WRITE, DEFAULT_DEVICE, message, &mLength);
			return NULL;
		} 
		else {
			cmcb = cmcb -> next;
		}
	
	}
	
	CMCB* op = cmcb->prev;
	CMCB* on = cmcb->next;
	op->next = on;
	on->prev = op;
	
	LMCB* oldlmcb = (LMCB*)((u32int)cmcb + (u32int)cmcb -> sizeBlock + (u32int)sizeof(CMCB));
	oldlmcb -> sizeBlock = (u32int)(oldlmcb -> sizeBlock - (u32int)allocSize);//breaks
	
	cmcb -> sizeBlock = cmcb -> sizeBlock - ((u32int)allocSize);
	
	CMCB* newcmcb = (CMCB*)((u32int)cmcb +cmcb->sizeBlock + (u32int)(sizeof(CMCB)) + (u32int)(sizeof(LMCB)));
	newcmcb->sizeBlock = (u32int)size;
	newcmcb->type = 1;
	newcmcb->beginAddress = (u32int)((u32int)newcmcb + (u32int)sizeof(CMCB));

	LMCB* newlmcb = (LMCB*)((u32int)newcmcb + (u32int)sizeof(CMCB) + (u32int)size);	
	newlmcb->type = 1;
	newlmcb->sizeBlock = (u32int)size;

	//INSERTS
	if(aIn(newcmcb) == 0){
		char* message10 = "could not insert allocated block into allocated list\n";
        	int mLength10 = strlen(message10);
        	sys_req(WRITE, DEFAULT_DEVICE, message10, &mLength10);
		return NULL;
	}
	if(freeIn(cmcb) == 0){
		char* message11 = "could not insert remaining free block into free list\n";
        	int mLength11 = strlen(message11);
        	sys_req(WRITE, DEFAULT_DEVICE, message11, &mLength11);
		return NULL;
	}
	
	//display start address of the allocated block as an integer
	// offset from the beginning of teh heap 
	// return a void* pointer to the allocated space

	int messageAddress = (int)newcmcb -> beginAddress - startHeapAddress;
	char* admess = "";
	
	itoa(messageAddress, admess, 10);
	int adml = strlen(admess);
	char* message1 = '\0';
	message1 = "Start address(offset heap start): ";
	int mLength1 = strlen(message1);
	sys_req(WRITE, DEFAULT_DEVICE, message1, &mLength1);
	sys_req(WRITE, DEFAULT_DEVICE, admess, &adml);
	return newcmcb -> beginAddress;

}
int aIn(CMCB* toInsert){
	CMCB* aiterate = allocHead;
	int flag1 = 0;
	if(!aiterate){
		allocHead = toInsert;
		toInsert->prev = NULL;
		toInsert->next = NULL;
		toInsert->type = 1;
		LMCB* til = (LMCB*)((u32int)toInsert + toInsert->sizeBlock + (u32int)(sizeof(CMCB)));
		til->type = 1;
		return 1;
	}
	if(toInsert->beginAddress < aiterate->beginAddress){ //if toInsert needs to be new head
		allocHead = toInsert;
		toInsert->next = aiterate;
		toInsert->prev = NULL;
		aiterate->prev = toInsert;
		flag1 = 1;
	}
	else{
		while(aiterate != NULL){
			if(toInsert->beginAddress < aiterate->beginAddress){
				CMCB* temp = aiterate->prev;
				temp->next = toInsert;
				aiterate->prev = toInsert;
				toInsert->prev = temp;
				toInsert->next = aiterate;
				flag1 = 1;
				break;
			}
			aiterate = aiterate->next;
		}
	}
	if(flag1 == 0){
		char* message = "could not insert into alloc list\n";
        	int mLength = strlen(message);
        	sys_req(WRITE, DEFAULT_DEVICE, message, &mLength);
		toInsert->type = 1;
		return 0;
	}

	toInsert->type = 1;
	LMCB* til = (LMCB*)((u32int)toInsert + toInsert->sizeBlock + (u32int)(sizeof(CMCB)));
	til->type = 1;
	return 1;
}
int isEmpty(){
	return allocHead == NULL;

}

void showA(){
	char* failed = "nothing was in the allocated list \n";
	int pbuff = 40;
	CMCB* temp = allocHead;
	
	if(!temp)
	{
		sys_req(WRITE, DEFAULT_DEVICE, failed, &pbuff);
		return;
	}
	char sizeOfBlockBuf[20];
	char address[20];
	int abuff = 50;
	int bbuff = 50;
	while(temp != NULL)
	{
		itoa((int)temp->sizeBlock, sizeOfBlockBuf,10);
		itoa((int)temp->beginAddress, address, 10); 
		strcat(address, " | ");
		strcat(sizeOfBlockBuf, "\n");
		sys_req(WRITE, DEFAULT_DEVICE, address, &abuff);
		sys_req(WRITE, DEFAULT_DEVICE, sizeOfBlockBuf, &bbuff);
		temp = temp->next;
	}

}
void showF(){
	char* failed = "nothing was in the free list \n";
	int pbuff = 40;
	CMCB* temp = freeHead;
	
	if(!temp)
	{
		sys_req(WRITE, DEFAULT_DEVICE, failed, &pbuff);
		return;
	}
	char sizeOfBlockBuf[20];
	char address[20];
	int abuff = 50;
	int bbuff = 50;
	while(temp != NULL)
	{
		itoa((int)temp->sizeBlock, sizeOfBlockBuf,10);
		itoa((int)temp->beginAddress, address, 10);
		strcat(address, " | ");
		strcat(sizeOfBlockBuf, "\n");
		sys_req(WRITE, DEFAULT_DEVICE, address, &abuff);
		sys_req(WRITE, DEFAULT_DEVICE, sizeOfBlockBuf, &bbuff);
		temp = temp->next;
	}

}

void heapHelp(){
	char* p ="$ ";
	int pl = strlen(p);
	char* message ="How big do you want the heap to be in bytes?";
	int mLength = strlen(message);
	sys_req(WRITE, DEFAULT_DEVICE, message, &mLength);
	sys_req(WRITE, DEFAULT_DEVICE, p, &pl);
	char inbuff[20];
	int ttt = 20;
	int bytez = sys_req(READ, DEFAULT_DEVICE, inbuff, &ttt);
	int flag = initializeHeap(bytez);
	if(flag == 0){
		char* message2 ="HEAP CREATION FAILED";
		int mLength2 = strlen(message2);
		sys_req(WRITE, DEFAULT_DEVICE, message2, &mLength2);
	}
	else{
		char* message3 ="HEAP CREATED";
		int mLength3 = strlen(message3);
		sys_req(WRITE, DEFAULT_DEVICE, message3, &mLength3);
	}
}

void* allocHelp(){
	if(!freeHead){
		char* message = "heap not yet init"; // end of list
			int mLength = strlen(message);
			sys_req(WRITE, DEFAULT_DEVICE, message, &mLength);
			return NULL;
	}
	char* p ="$ ";
	int ttt = 20;
	int pl = strlen(p);
	char* message ="How much do you want to allocate in bytes?";
	int mLength = strlen(message);
	sys_req(WRITE, DEFAULT_DEVICE, message, &mLength);
	sys_req(WRITE, DEFAULT_DEVICE, p, &pl);
	char inbuff[20];
	int bytez = sys_req(READ, DEFAULT_DEVICE, inbuff, &ttt);
	void* ret = (void*)(allocMem(bytez));
	if(ret == NULL){
		char* message2 ="ALLOCATION FAILED";
		int mLength2 = strlen(message2);
		sys_req(WRITE, DEFAULT_DEVICE, message2, &mLength2);
	}
	return ret;
}

void freeHelp(){
	char* p ="$ ";
	int ttt = 20;
	int pl = strlen(p);
	char* message ="What is the address you want to free?";
	int mLength = strlen(message);
	sys_req(WRITE, DEFAULT_DEVICE, message, &mLength);
	sys_req(WRITE, DEFAULT_DEVICE, p, &pl);
	char inbuff[20];
	int addr = sys_req(READ, DEFAULT_DEVICE, inbuff, &ttt);
	u32int* inn;
	inn = (u32int*) addr;
	int flag = freeMem(inn);
	if(flag == 0){
		char* message2 ="FAILED TO FREE";
		int mLength2 = strlen(message2);
		sys_req(WRITE, DEFAULT_DEVICE, message2, &mLength2);
	}
	else{
		char* message3 ="\nSUCCESSFUL FREE\n";
		int mLength3 = strlen(message3);
		sys_req(WRITE, DEFAULT_DEVICE, message3, &mLength3);
	}
}

void merge(){
	CMCB* toInsert = freeHead;
	while(toInsert){
		CMCB* tn = toInsert->next;
		CMCB* tp = toInsert->prev;
		if(tn){

			if(toInsert->beginAddress + toInsert->sizeBlock +(u32int)(sizeof(CMCB)) + (u32int)(sizeof(LMCB)) == tn->beginAddress ){

				toInsert->sizeBlock = toInsert->sizeBlock + tn->sizeBlock + (u32int)(sizeof(CMCB)) + (u32int)(sizeof(LMCB));
				toInsert->next = tn->next;
				if(tn->next != NULL){
					tn->next->prev = toInsert;
				}
			}
		}

		if(tp){
	
			if(tp->beginAddress + tp->sizeBlock +(u32int)(sizeof(CMCB)) + (u32int)(sizeof(LMCB))== toInsert->beginAddress){
				tp->sizeBlock = tp->sizeBlock + toInsert->sizeBlock + (u32int)(sizeof(CMCB)) + (u32int)(sizeof(LMCB));
				tp->next = toInsert->next;
				if(toInsert->next != NULL){
					toInsert->next->prev = tp;
				}
			}
		}
		toInsert = toInsert->next;
	}
}
