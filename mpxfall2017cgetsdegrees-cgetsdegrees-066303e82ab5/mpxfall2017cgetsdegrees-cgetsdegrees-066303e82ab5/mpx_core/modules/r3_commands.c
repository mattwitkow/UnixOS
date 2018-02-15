#include "r3_commands.h"
#include "r2_commands.h"
#include <core/serial.h>
#include <core/io.h>
#include <system.h>
#include <string.h>
#include "mpx_supt.h"
#include <stdint.h>
#include "procsr3.h"
PCB* cop = NULL;					//global pcb for currently operating process
context* oldContext;					
param params; 
void loadr3(){
	CreatePCB("new_pcb1", 1, 2);
	SuspendPCB("new_pcb1"); //making sure new pcb is suspended
	context* cp = (context*)((FindPCB("new_pcb1"))->stackTop);
	memset(cp, 0, sizeof(context));
	cp->fs = 0x10;
	cp->gs = 0x10;
	cp->ds = 0x10;
	cp->es = 0x10;
	cp->cs = 0x8;
	cp->ebp = (u32int)((FindPCB("new_pcb1"))->stack); 
	cp->esp = (u32int)((FindPCB("new_pcb1"))->stackTop);
	cp->eip = (u32int)proc1; //The function correlating to the process, ie. Proc1
	cp->eflags = 0x22;
	

	CreatePCB("new_pcb2", 1, 3);
	SuspendPCB("new_pcb2"); //making sure new pcb is suspended
	context* cp2 = (context*)((FindPCB("new_pcb2"))->stackTop);
	memset(cp2, 0, sizeof(context));
	cp2->fs = 0x10;
	cp2->gs = 0x10;
	cp2->ds = 0x10;
	cp2->es = 0x10;
	cp2->cs = 0x8;
	cp2->ebp = (u32int)((FindPCB("new_pcb2"))->stack); 
	cp2->esp = (u32int)((FindPCB("new_pcb2"))->stackTop);
	cp2->eip = (u32int)proc2; 
	cp2->eflags = 0x22;
	

	CreatePCB("new_pcb3", 1, 4);
	SuspendPCB("new_pcb3"); //making sure new pcb is suspended
	context* cp3 = (context*)((FindPCB("new_pcb3"))->stackTop);
	memset(cp3, 0, sizeof(context));
	cp3->fs = 0x10;
	cp3->gs = 0x10;
	cp3->ds = 0x10;
	cp3->es = 0x10;
	cp3->cs = 0x8;
	cp3->ebp = (u32int)((FindPCB("new_pcb3"))->stack); 
	cp3->esp = (u32int)((FindPCB("new_pcb3"))->stackTop);
	cp3->eip = (u32int)proc3; 
	cp3->eflags = 0x22;
	

	CreatePCB("new_pcb4", 1, 5);
	SuspendPCB("new_pcb4"); //making sure new pcb is suspended
	context* cp4 = (context*)((FindPCB("new_pcb4"))->stackTop);
	memset(cp4, 0, sizeof(context));
	cp4->fs = 0x10;
	cp4->gs = 0x10;
	cp4->ds = 0x10;
	cp4->es = 0x10;
	cp4->cs = 0x8;
	cp4->ebp = (u32int)((FindPCB("new_pcb4"))->stack); 
	cp4->esp = (u32int)((FindPCB("new_pcb4"))->stackTop);
	cp4->eip = (u32int)proc4; 
	cp4->eflags = 0x22;
	
	CreatePCB("new_pcb5", 1, 6);
	SuspendPCB("new_pcb5"); //making sure new pcb is suspended
	context* cp5 = (context*)((FindPCB("new_pcb5"))->stackTop);
	memset(cp5, 0, sizeof(context));
	cp5->fs = 0x10;
	cp5->gs = 0x10;
	cp5->ds = 0x10;
	cp5->es = 0x10;
	cp5->cs = 0x8;
	cp5->ebp = (u32int)((FindPCB("new_pcb5"))->stack); 
	cp5->esp = (u32int)((FindPCB("new_pcb5"))->stackTop);
	cp5->eip = (u32int)proc5;
	cp5->eflags = 0x22;
	
}

void yield(){
	asm volatile("int $60");
}

u32int* sys_call(context* registers){
	if(cop == NULL)					//check if sys_call has been called before
		oldContext = registers;			//DOUBLE CHECK FOR ME: oldContext now point to whatever registers what pointing to
	else
	{
		if(params.op_code == IDLE)
		{
			
			cop->stackTop = (unsigned char*) registers;
			 //probably on the right track, but not 100% sure
			cop->state = 1; //from running to ready
			InsertPCB(cop);
		}
		if(params.op_code == EXIT)
			FreePCB(cop);
			cop = NULL;
	}		
	if(ready->count > 0)
	{	PCB* removed = ready->head;
		RemovePCB(removed);
		removed->state = 0;
		cop = removed;				//DOUBLE CHECK FOR ME: Pointers are hard
		return (u32int*)cop->stackTop;

	}
	return (u32int*) oldContext;

}

