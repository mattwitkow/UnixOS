#include "r2_commands.h"
#include "r3_commands.h"
#include <core/serial.h>
#include <core/io.h>
#include <system.h>
#include <string.h>
#include "mpx_supt.h"

void initQs(){
	ready = (struct Queue*) sys_alloc_mem(sizeof(struct Queue));
	ready->count = 0;
	ready->head->next = ready->tail;
	ready->tail->prev = ready->head;
	blocked = (struct Queue*) sys_alloc_mem(sizeof(struct Queue));
	blocked->count = 0;
	blocked->head->next = blocked->tail;
	blocked->tail->prev = blocked->head;
}
struct PCB* AllocatePCB() {
	struct PCB* pcb;
	pcb = (struct PCB*) sys_alloc_mem(sizeof(struct PCB));

	if(pcb == NULL) {
		return NULL;
	}
	return pcb;
}

struct PCB* SetupPCB(char* name, int class, int priority) {
	struct PCB* pcb = AllocatePCB();

	if(pcb == NULL) {
		char* message ="error allocating pcb";
		int mLength = strlen(message);
		sys_req(WRITE, DEFAULT_DEVICE, message, &mLength);
	}

	pcb->stack = (unsigned char*) sys_alloc_mem(1024 * sizeof(unsigned char*));

	char* pName = pcb->processName;
	strcpy(pName, name);
	pcb->processClass = class;
	pcb->priority = priority;
	pcb->state = 1;
	pcb->isSuspended = 0;
	pcb->stackBase = pcb->stack;
	pcb->stackTop = pcb->stackBase + 1024 - sizeof(struct context);
	pcb->next = NULL;
	pcb->prev = NULL;
	return pcb;
}

int FreePCB(PCB* pcbToFree){
	
	if(pcbToFree == NULL){
		//char* message ="That PCB was null";
		//int mLength = strlen(message);
		//sys_req(WRITE, DEFAULT_DEVICE, message, &mLength);
		return 0;
	}

	sys_free_mem(pcbToFree);
	return 1;
}
int isValidProcessName(char* processName)
{
	

		
	if(!processName)
		return NULL;
	if(strlen(processName) > 20)//could be 19. have to test
		return -1;
	return 1;
}
void ShowPCB(char* processName)
{
	//char* failed = "invalid process name was entered\n";
	//int pbuff = 40;
	////if(!isValidProcessName(processName))
	//{
	//	sys_req(WRITE, DEFAULT_DEVICE, failed, &pbuff);
	//	return;
	//}
	int retbuff = 200;
	PCB* show = FindPCB(processName);
	char* failednull = "NULL\n";
	int nbuff = 40;
	if(!show){
		sys_req(WRITE, DEFAULT_DEVICE, failednull, &nbuff);
		return;
		}
	char printMessage[200] = "Process Name: "; 
	strcat(printMessage, show->processName);
	strcat(printMessage, "\nProcess Class: ");
	char itoa1[20];
	itoa(show->processClass,itoa1,10);
	strcat(printMessage, itoa1);
	strcat(printMessage, "\nProcess State(running/ready/blocked): ");
	if(show->state == 0){
		strcat(printMessage, "running");
	}
	else if(show->state == 1){
		strcat(printMessage, "ready");
	}
	else{
		strcat(printMessage, "blocked");
	}
	strcat(printMessage,"\nProcess State(suspended/not suspended): ");
	if(show->isSuspended)	
		strcat(printMessage, "Suspended");
	else
		strcat(printMessage, "Not suspended");
	strcat(printMessage, "\nProcess Priority: ");
	char itoa2[20];
	itoa(show->priority,itoa2,10);
	strcat(printMessage, itoa2);	
	strcat(printMessage, "\n");
	sys_req(WRITE, DEFAULT_DEVICE, printMessage, &retbuff);	
}

void ShowHelp(){
	int buff = 50;
	char pmessage[50] = "Please enter the PCB to show\n";
	char *prompt = "$ ";
	sys_req(WRITE, DEFAULT_DEVICE, pmessage, &buff);
	sys_req(WRITE, DEFAULT_DEVICE, prompt, &buff);

	char namebuff[50];
	sys_req(READ, DEFAULT_DEVICE, namebuff, &buff);
	ShowPCB(namebuff);
	
}

void ShowReady()
{
	char* failed = "nothing was in the ready queue\n";
	int pbuff = 40;
	if(ready->count == 0)
	{
		sys_req(WRITE, DEFAULT_DEVICE, failed, &pbuff);
		return;
	}
	PCB* temp = ready->head;
	while(temp != NULL)
	{	
		ShowPCB(temp->processName);
		temp = temp->next;
		
	}
		
}/*
void ShowReadyWithPointers()
{
	char* failed = "nothing was in the ready queue\n";
	int pbuff = 40;
	serial_println(ready->head->processName);
	serial_println(ready->head->prev->processName);
	serial_println(ready->head->next->processName);
	char buff[20];
	serial_println(itoa(ready->count,buff,10) );
	if(ready->count < 1 )
	{
		sys_req(WRITE, DEFAULT_DEVICE, failed, &pbuff);
		serial_println("here1");
		return;
	}
	serial_println("here2");
	PCB* temp = ready->head;
	while(temp != NULL)
	{	
		ShowPCB(temp->prev->processName);
		ShowPCB(temp->processName);
		ShowPCB(temp->next->processName);
		temp = temp->next;
		
	}
		
}
*/
char** tempBlocked;
int tempBlockedSize = 0;
void BlockAllReady()
{
	char* failed = "nothing was in the ready queue\n";
	int pbuff = 40;
	if(!ready)
	{
		sys_req(WRITE, DEFAULT_DEVICE, failed, &pbuff);
		return;
	}
	PCB* temp = ready->head;
	if(!temp)
		return;
	int i = 0;
	tempBlocked[i] = temp->processName;
	i++;
	
	while(temp!= NULL)
	{
		//BlockPCB(temp->processName);
		temp = temp->next;
		tempBlocked[i] = temp->processName;
		i++;
		tempBlockedSize++;
	}
	int j = 0;
	while(tempBlocked[j]!= NULL)
	{
		BlockPCB(tempBlocked[j]);
		temp = temp->next;
		//tempBlocked[i] = temp->processName;
		//i++;
		j++;
	}
	
}
void PutBackToReady(PCB* ptr){
	if(!ptr)
		return;
	if(ready->head==NULL)
	{
	ptr->prev = ptr->next = NULL ;
        ready->head = ready->tail= ptr ;
	}
	else
	{
		ptr->prev = ready->tail;
        ready->tail->next = ptr ;
		ptr->next= NULL;
		ready->tail=ptr;
	}

}
void UnblockAllReady()
{
	//char failed[40];
	//itoa(tempBlockedSize,failed ,10);
	//int pbuff = 40;
	//sys_req(WRITE, DEFAULT_DEVICE, failed, &pbuff);
	int i = 0;
	for (i = 0; i < tempBlockedSize;i++)
	{
		//char failed[40];
		//int buff = 40;
		//sys_req(WRITE, DEFAULT_DEVICE, tempBlocked[i], &buff);
		//PutBackToReady(FindPCB(tempBlocked[i]));
		//RemovePCB(FindPCB(tempBlocked[i]));
		PCB*thing = RemoveReturnPCB(FindPCB(tempBlocked[i]));
		thing->state = 1;
		PutBackToReady(thing);
		

	}
	//ShowReady();



/*


	while(i < tempBlockedSize)
	{
		UnblockPCB(tempBlocked[i]);
		i++;	

	}*/
}






void ShowBlocked()
{
	
	char* failed = "nothing was in the blocked queue\n";
	int pbuff = 40;
	if(!blocked)
	{
		sys_req(WRITE, DEFAULT_DEVICE, failed, &pbuff);
		return;
	}
	PCB* temp = blocked->head;
	while(temp != NULL)
	{
		ShowPCB(temp->processName);
		temp = temp->next;
	}
		
}
void ShowAll()
{
	ShowReady();
	ShowBlocked();
}

int CreatePCB(char* name, int class, int priority) {
	char* failed = "Sorry, that process already exists\n";
	int pbuff = 40;
	if(ready)
	{
		PCB* temp = ready->head;
		while(temp != NULL)
		{
			if(!(strcmp(temp->processName, name)))
			{
				sys_req(WRITE, DEFAULT_DEVICE, failed, &pbuff);
				FreePCB(temp);
				return 0;
			}
			temp = temp->next;
		}
	FreePCB(temp);
	}
	
	if(blocked)
	{
		PCB* temp2 = blocked->head;
		while(temp2 != NULL)
		{
			if(!(strcmp(temp2->processName, name)))
			{
				sys_req(WRITE, DEFAULT_DEVICE, failed, &pbuff);
				FreePCB(temp2);
				return 0;
			}
			temp2 = temp2->next;
		}
	FreePCB(temp2);
	}
	
	
	struct PCB* pcb = SetupPCB(name, class, priority);
	if(pcb == NULL) {
		char* message = "pcb error\n";
        int mLength = strlen(message);
        sys_req(WRITE, DEFAULT_DEVICE, message, &mLength);
        return 0;
	}
	else {
		InsertPCB(pcb);
		char* message = "pcb created\n";
        int mLength = strlen(message);
        sys_req(WRITE, DEFAULT_DEVICE, message, &mLength);
	}
	return 1;
}
int CreateHelp(){
	
	int buff = 100;
	char inbuff[100];
	char pmessage[100] = "Please enter a name for the PCB\n";
	char pmessage2[100] = "Please enter a the PCB's class: 0 for app, 1 for system\n";
	char pmessage3[100] = "Please enter a the PCB's priority (0-9 with 9 as high)\n";
	char *prompt = "$ ";

	sys_req(WRITE, DEFAULT_DEVICE, pmessage, &buff);
	sys_req(WRITE, DEFAULT_DEVICE, prompt, &buff);

	char namebuff[50];
	sys_req(READ, DEFAULT_DEVICE, namebuff, &buff);
	if(strlen(namebuff) < 1 || strlen(namebuff) > 20) {
		char* message = "name must be 1-20 chars\n";
        	int mLength = strlen(message);
        	sys_req(WRITE, DEFAULT_DEVICE, message, &mLength);
        	return 0;
	}
	
	sys_req(WRITE, DEFAULT_DEVICE, pmessage2, &buff);
	sys_req(WRITE, DEFAULT_DEVICE, prompt, &buff);

	int class = sys_req(READ, DEFAULT_DEVICE, inbuff, &buff);
	if(!(class == 0 || class == 1)) {
		char* message = "class must be 0 or 1\n";
        	int mLength = strlen(message);
        	sys_req(WRITE, DEFAULT_DEVICE, message, &mLength);
        	return 0;
	}

	sys_req(WRITE, DEFAULT_DEVICE, pmessage3, &buff);
	sys_req(WRITE, DEFAULT_DEVICE, prompt, &buff);
	int prio = sys_req(READ, DEFAULT_DEVICE, inbuff, &buff);
	if(prio < 0 || prio > 9) {
		char* message = "priority must be 0-9\n";
        	int mLength = strlen(message);
        	sys_req(WRITE, DEFAULT_DEVICE, message, &mLength);
        	return 0;
	}

	int ret = CreatePCB(namebuff, class, prio);
	return ret;
}
struct PCB* FindPCB(char* processName){
	
	//char* message = "process name was not in system or invalid process name\n";
        	//int mLength = strlen(message);
	if(!isValidProcessName(processName)){
        	//sys_req(WRITE, DEFAULT_DEVICE, message, &mLength);
		return NULL;
	}
	struct PCB* ret = ready->head;

	while(ret != NULL){
		if(strcmp((ret->processName), processName) == 0){
			return ret;
		}
		ret = ret->next;
	}
	ret = blocked->head;
	while(ret != NULL){
		if(strcmp((ret->processName), processName) == 0){
			return ret;
		}
		ret = ret->next;
	}
	//sys_req(WRITE, DEFAULT_DEVICE, message, &mLength);
	return NULL;
}

void InsertPCB(PCB* pcbToInsert){
	if(pcbToInsert == NULL){
		//char* message ="could not insert pcb\n";
		//int mLength = strlen(message);
		//sys_req(WRITE, DEFAULT_DEVICE, message, &mLength);
		return;
	}
	if(FindPCB(pcbToInsert->processName) != NULL){
		//char* message ="could not insert pcb\n";
		//int mLength = strlen(message);
		//sys_req(WRITE, DEFAULT_DEVICE, message, &mLength);
		return;
	}
	if(pcbToInsert->state == 1){
		struct PCB* temp = ready->head;
		struct PCB* temp2 = NULL;
		if(ready->head == NULL) {  //queue is empty
			ready->head = pcbToInsert;
			ready->tail = pcbToInsert;
			pcbToInsert->next = NULL;
			pcbToInsert->prev = NULL;
		}
		
		else{
			if(ready->head == ready->tail) { //1 element
				if(pcbToInsert->priority > temp->priority) {
					pcbToInsert->next = temp;
					pcbToInsert->prev = NULL;
					temp->next = NULL;
					temp->prev = pcbToInsert;
					ready->head = pcbToInsert;
					ready->tail = temp;
					
				}
				else {
					pcbToInsert->next = NULL;
					pcbToInsert->prev = temp;
					temp->next = pcbToInsert;
					temp->prev = NULL;
					ready->head = temp;
					ready->tail = pcbToInsert;
					
				}
			}
			else { //2 or more elements
				while(temp != NULL){
					if(pcbToInsert->priority > temp->priority) {
						pcbToInsert->next = temp;
						temp2 = temp->prev;
						pcbToInsert->prev = temp2;
						temp2->next = pcbToInsert;
						temp->prev = pcbToInsert;
						if(temp == ready->head) {
							ready->head = pcbToInsert;
						}
						
						break;
					}
					else {
						temp = temp->next;	
					}
				}
			}
			
		}
		ready->count++;
	}
	else{
		if(blocked->head == NULL) {
			blocked->head = pcbToInsert;
			blocked->tail = pcbToInsert;
		}
		else if(blocked->head == blocked->tail) {
			pcbToInsert->next = NULL;
			pcbToInsert->prev = blocked->head;
			blocked->tail = pcbToInsert;
			blocked->head->next = pcbToInsert;
		}
		else {
			blocked->tail->next = pcbToInsert;
			pcbToInsert->next = NULL;
			pcbToInsert->prev = blocked->tail;
			blocked->tail = pcbToInsert;
		}
		blocked->count++;
	}
}	
int RemovePCB(PCB* pcbToRemove){
	int buff = 50;
	char pmessage[50] = "PCB to be removed not found\n";
	
	if(!FindPCB(pcbToRemove->processName))
	{
		sys_req(WRITE, DEFAULT_DEVICE, pmessage, &buff);
		return -1;
	}
	int flagA = 0;
	int flagB = 0;
	if(FindPCB(pcbToRemove->processName)->state == 1)
		flagA = 1;
	if(FindPCB(pcbToRemove->processName)->state == 2)
		flagB = 1;
	if(ready->head == pcbToRemove) {
		if(ready->count == 1){	//if there was only one thing in the ready queue to remove
			ready->head->next = NULL;
			ready->head->prev = NULL;
			ready->head = NULL;
			ready->count--;
			return 1;
		}
		ready->head = pcbToRemove->next;
	}

	if(blocked->head == pcbToRemove) {
		if(blocked->count == 1){	//if there was only one thing in the blocked queue to remove
			blocked->head->next = NULL;
			blocked->head->prev = NULL;
			blocked->head = NULL;
			blocked->count--;
			return 1;
		}
		blocked->head = pcbToRemove->next;
	}
	
	if(!(pcbToRemove->next == NULL))
		pcbToRemove->next->prev = pcbToRemove->prev;
	if(!(pcbToRemove->prev == NULL))
		pcbToRemove->prev->next = pcbToRemove->next;
	if (flagA ==1)
		ready->count--;
	if (flagB == 1)
		blocked->count--;
	return 1;
}
PCB* RemoveReturnPCB(PCB* pcbToRemove){	
	
	if(ready->head == pcbToRemove) {
		ready->head = pcbToRemove->next;
		ready->count--;
	}

	if(blocked->head == pcbToRemove) {
		blocked->head = pcbToRemove->next;
		blocked->count--;
	}

	pcbToRemove->next->prev = pcbToRemove->prev;
	pcbToRemove->prev->next = pcbToRemove->next;
	pcbToRemove->next = NULL;
	pcbToRemove->prev = NULL;
	return pcbToRemove;
}
int MoveToFront(char* processName){
 PCB* buildFrom = FindPCB(processName);
	if(!buildFrom)
		return 0;
	if(strcmp(ready->head->processName,processName) == 0)
		return 1;
 if(strcmp(ready->tail->processName,processName) == 0)
    {
      buildFrom->prev->next = NULL;
      ready->tail = buildFrom->prev;
    }
  else // n is in the middle of list
    {
      buildFrom->prev->next = buildFrom->next;
      buildFrom->next->prev = buildFrom->prev;
    }

  // make n the head of the list
  ready->head->prev = buildFrom;
  buildFrom->next = ready->head;
  buildFrom->prev = NULL;
  ready->head = buildFrom;
return 1;
}
PCB* DQReady(){	
	if(ready) {
		PCB* ret = ready->head;
		ready->head = ready->head->next;
		ready->count--;
		ready->head->next->prev = ready->head->prev;
	ready->head->prev->next = ready->head->next;
	ready->head->next = NULL;
	ready->head->prev = NULL;
	return ret;
	}
/*
	if(blocked) {
		PCB* ret = blocked->head;
		blocked->head = pcbToRemove->next;
		blocked->count--;
		pcbToRemove->next->prev = pcbToRemove->prev;
	pcbToRemove->prev->next = pcbToRemove->next;
	pcbToRemove->next = NULL;
	pcbToRemove->prev = NULL;
	return ret;
	}
*/
	char* failed = "Sorry, nothing in queue to dequeue\n";
	int pbuff = 40;
	sys_req(WRITE, DEFAULT_DEVICE, failed, &pbuff);
	
	return NULL;
}




int BlockPCB(char* processName) {
	char* failed = "Sorry, that process is already blocked\n";
	int pbuff = 40;
	if(blocked)
	{
		PCB* temp = blocked->head;
		while(temp != NULL)
		{
			if(!(strcmp(temp->processName, processName)))
			{
				sys_req(WRITE, DEFAULT_DEVICE, failed, &pbuff);
				FreePCB(temp);
				return 0;
			}
			temp = temp->next;
		}
	FreePCB(temp);
	}


	struct PCB* pcb = FindPCB(processName);
	if(pcb == NULL) {
		char* message ="could not find pcb\n";
		int mLength = strlen(message);
		sys_req(WRITE, DEFAULT_DEVICE, message, &mLength);
		return 0;
	}
	RemovePCB(pcb);
	pcb->state = 2;
	InsertPCB(pcb);
	return 1;
}
int BlockHelp(){
	int buff = 50;
	char pmessage[50] = "Please enter the PCB to Block\n";
	char *prompt = "$ ";
	sys_req(WRITE, DEFAULT_DEVICE, pmessage, &buff);
	sys_req(WRITE, DEFAULT_DEVICE, prompt, &buff);

	char namebuff[50];
	sys_req(READ, DEFAULT_DEVICE, namebuff, &buff);
	int ret = BlockPCB(namebuff);
	return ret;
}
int UnblockPCB(char* processName) {
	struct PCB* pcb = FindPCB(processName);
	if(pcb == NULL) {
		char* message ="could not find pcb\n";
		int mLength = strlen(message);
		sys_req(WRITE, DEFAULT_DEVICE, message, &mLength);
		return 0;
	}
	char* message ="sorry, PCB was already unblocked \n";
	int mLength = strlen(message);
	if(pcb->state == 1){
		sys_req(WRITE, DEFAULT_DEVICE, message, &mLength);
		return 0;
	}


	RemovePCB(pcb);
	pcb->state = 1;
	InsertPCB(pcb);
	return 1;
}
int UnblockHelp(){
	int buff = 50;
	char pmessage[50] = "Please enter the PCB to Unblock\n";
	char *prompt = "$ ";
	sys_req(WRITE, DEFAULT_DEVICE, pmessage, &buff);
	sys_req(WRITE, DEFAULT_DEVICE, prompt, &buff);

	char namebuff[50];
	sys_req(READ, DEFAULT_DEVICE, namebuff, &buff);
	int ret = UnblockPCB(namebuff);
	return ret;
}
int DeletePCB(char* processName){
	char* message ="could not delete pcb \n";
	int mLength = strlen(message);
	if(FindPCB(processName) == NULL){
		sys_req(WRITE, DEFAULT_DEVICE, message, &mLength);
		return 0;
	}

	PCB* deleteMe = FindPCB(processName);
	RemovePCB(deleteMe);
	FreePCB(deleteMe);

	return 1;
}
int DeleteHelp(){
	int buff = 50;
	char pmessage[50] = "Please enter the PCB to Delete\n";
	char *prompt = "$ ";
	sys_req(WRITE, DEFAULT_DEVICE, pmessage, &buff);
	sys_req(WRITE, DEFAULT_DEVICE, prompt, &buff);

	char namebuff[50];
	sys_req(READ, DEFAULT_DEVICE, namebuff, &buff);
	int ret = DeletePCB(namebuff);
	return ret;
}
int SuspendPCB(char* processName){

	char* message ="could not suspend pcb \n";
	int mLength = strlen(message);

	struct PCB* pcb = FindPCB(processName);

	if(!(pcb)){
		sys_req(WRITE, DEFAULT_DEVICE, message, &mLength);
		return 0;
	}
	char* message2 ="sorry, PCB was already suspended \n";
	int mLength2 = strlen(message2);
	if(pcb->isSuspended == 1){
		sys_req(WRITE, DEFAULT_DEVICE, message2, &mLength2);
		return 0;
	}
	pcb->isSuspended = 1;
	return 1;
}

int SuspendHelp(){
	int buff = 50;
	char pmessage[50] = "Please enter the PCB to suspend\n";
	char *prompt = "$ ";
	sys_req(WRITE, DEFAULT_DEVICE, pmessage, &buff);
	sys_req(WRITE, DEFAULT_DEVICE, prompt, &buff);

	char namebuff[50];
	sys_req(READ, DEFAULT_DEVICE, namebuff, &buff);
	int ret = SuspendPCB(namebuff);
	return ret;
}

int ResumePCB(char* processName){

	char* message ="could not resume pcb\n";
	int mLength = strlen(message);
	struct PCB* pcb = FindPCB(processName);
	if(!(pcb)){
		sys_req(WRITE, DEFAULT_DEVICE, message, &mLength);
		return 0;
	}
	char* message2 ="sorry, PCB was already resumed \n";
	int mLength2 = strlen(message2);
	if(pcb->isSuspended == 0){
		sys_req(WRITE, DEFAULT_DEVICE, message2, &mLength2);
		return 0;
	}
	pcb->isSuspended = 0;
	return 1;
}




int ResumeHelp(){
	int buff = 50;
	int buff2 =100;
	char pmessage[50] = "Please enter the PCB to resume\n";
	char fmessage[100] = "why are you trying to resume null or not ready processes?\n";
	char *prompt = "$ ";
	sys_req(WRITE, DEFAULT_DEVICE, pmessage, &buff);
	sys_req(WRITE, DEFAULT_DEVICE, prompt, &buff);

	char namebuff[50];
	sys_req(READ, DEFAULT_DEVICE, namebuff, &buff);

	PCB* ret = FindPCB(namebuff);
	if(!ret || ret->state != 1){
		sys_req(WRITE,DEFAULT_DEVICE,fmessage,&buff2);
		return 0;

	}
	
	ret->isSuspended = 0;	
	MoveToFront(ret->processName);
	asm volatile ("int $60");
	return 1;
}
/*
PCB* ret = RemoveReturnPCB(namebuff);
	
	if(!ret)
		return 0;
	else
		return 1
*/
int SetPCBPriority(char* processName, int newPriority){

	struct PCB* pcb = FindPCB(processName);
	int flag = RemovePCB(pcb);
	pcb->next = NULL;
	pcb->prev = NULL;
	if(flag == 1) {
		pcb->priority = newPriority;
		InsertPCB(pcb);
		return 1;
	}
	return 0;
}

int PriorityHelp(){
	int buff = 50;
	char inbuff[50];
	char pmessage[50] = "Please enter the PCB to set the priority of\n";
	char pmessage2[50] = "Please enter the new priority\n";
	char *prompt = "$ ";
	sys_req(WRITE, DEFAULT_DEVICE, pmessage, &buff);
	sys_req(WRITE, DEFAULT_DEVICE, prompt, &buff);

	char namebuff[50];
	sys_req(READ, DEFAULT_DEVICE, namebuff, &buff);

	sys_req(WRITE, DEFAULT_DEVICE, pmessage2, &buff);
	sys_req(WRITE, DEFAULT_DEVICE, prompt, &buff);

	int prio = sys_req(READ, DEFAULT_DEVICE, inbuff, &buff);
	if(!(prio >= 0 && prio <= 9)){
		char* message ="priority must be from 0 to 9\n";
		int mLength = strlen(message);
		sys_req(WRITE, DEFAULT_DEVICE, message, &mLength);
		return 0;
	}
	int ret = SetPCBPriority(namebuff, prio);
	return ret;
}
