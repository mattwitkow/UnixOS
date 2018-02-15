//header file for r3
#ifndef R3_COMMANDS_H_
#define R3_COMMANDS_H_
#include <system.h>
#include "r2_commands.h"

typedef struct context{
	u32int gs, fs, es, ds;
	u32int edi, esi, ebp, esp, ebx, edx, ecx, eax;
	u32int eip, cs, eflags;
}context;

u32int* sys_call(context* registers); 

void yield();

void loadr3();


#endif
