#include "r1_input.h"
#include "r1_commands.h"
#include "r2_commands.h"
#include "r3_commands.h"
#include <core/serial.h>
#include <core/io.h>
#include <string.h>
#include "mpx_supt.h"
#include <stdint.h>
int bigflag = 1;

int comHand(int insideCommand) {
    if(bigflag){
	initQs();
	bigflag=0;
    }
    char cmdBuffer[100];
    int bufferSize;
    int quit = 0; 
    char* prompt = "$ "; 
    int pBuffer = 2;
    sys_req(WRITE, DEFAULT_DEVICE, prompt, &pBuffer);
    while (!quit) {
        bufferSize = 99;

        int command = sys_req(READ, DEFAULT_DEVICE, cmdBuffer, &bufferSize);
        if (command == 106) {
            int confirm = shutdown();
            if(confirm == 4){
                quit = 1;
            }
            else {
                sys_req(WRITE, DEFAULT_DEVICE, prompt, &pBuffer);
            }
        }
        else if (command == 107) {
            help();
            sys_req(WRITE, DEFAULT_DEVICE, prompt, &pBuffer);
        }
        else if (command == 108) {
            //serial_println("version command");
		version();
            sys_req(WRITE, DEFAULT_DEVICE, prompt, &pBuffer);
        }
        else if (command == 109) {
            getDate();
            sys_req(WRITE, DEFAULT_DEVICE, prompt, &pBuffer);
        }
        else if (command == 110) {
            setDate();
            sys_req(WRITE, DEFAULT_DEVICE, prompt, &pBuffer);
        }
        else if (command == 111) {
            getTime();
            sys_req(WRITE, DEFAULT_DEVICE, prompt, &pBuffer);
        }
        else if (command == 112) {
            setTime();
            sys_req(WRITE, DEFAULT_DEVICE, prompt, &pBuffer);
        }
	else if (command == 113) {
            SuspendHelp();
            sys_req(WRITE, DEFAULT_DEVICE, prompt, &pBuffer);
        }
	else if (command == 114) {
            ResumeHelp();
            sys_req(WRITE, DEFAULT_DEVICE, prompt, &pBuffer);
        }
	else if (command == 115) {
            PriorityHelp();
            sys_req(WRITE, DEFAULT_DEVICE, prompt, &pBuffer);
        }
	else if (command == 116) {
            ShowHelp();
            sys_req(WRITE, DEFAULT_DEVICE, prompt, &pBuffer);
        }
	else if (command == 117) {
            ShowAll();
            sys_req(WRITE, DEFAULT_DEVICE, prompt, &pBuffer);
        }
	else if (command == 118) {
            ShowBlocked();
            sys_req(WRITE, DEFAULT_DEVICE, prompt, &pBuffer);
        }
	else if (command == 119) {
            ShowReady();
            sys_req(WRITE, DEFAULT_DEVICE, prompt, &pBuffer);
        }
	else if (command == 120) {
            CreateHelp();
	    sys_req(WRITE, DEFAULT_DEVICE, prompt, &pBuffer);
        }
	else if (command == 121) {
            DeleteHelp();
            sys_req(WRITE, DEFAULT_DEVICE, prompt, &pBuffer);
        }
	else if (command == 122) {
            BlockHelp();
            sys_req(WRITE, DEFAULT_DEVICE, prompt, &pBuffer);
        }
	else if (command == 123) {
            UnblockHelp();
            sys_req(WRITE, DEFAULT_DEVICE, prompt, &pBuffer);
        }
	else if (command == 124){
		yield();
		sys_req(WRITE, DEFAULT_DEVICE, prompt, &pBuffer);
	}
	else if (command == 125){
		loadr3();
		sys_req(WRITE, DEFAULT_DEVICE, prompt, &pBuffer);
	}
        else if (command == 104 && insideCommand == 0) {
            char* message = "invalid command\n";
            int mLength = strlen(message);
            sys_req(WRITE, DEFAULT_DEVICE, message, &mLength);
            sys_req(WRITE, DEFAULT_DEVICE, prompt, &pBuffer);
        }
        else if (command == 105 && insideCommand == 0) {
            char* message = "invalid command\n";
            int mLength = strlen(message);
            sys_req(WRITE, DEFAULT_DEVICE, message, &mLength);
            sys_req(WRITE, DEFAULT_DEVICE, prompt, &pBuffer);
        }
        else if (command == 104 && insideCommand == 1) {
            return 104;
        }
        else if (command == 105 && insideCommand == 1) {
            return 105;
        }
	else if (command < 100 && insideCommand == 0){
		char* message = "invalid command\n";
            int mLength = strlen(message);
            sys_req(WRITE, DEFAULT_DEVICE, message, &mLength);
            sys_req(WRITE, DEFAULT_DEVICE, prompt, &pBuffer);
	}
	else if (command == 666 && insideCommand == 0){
		char* message = "invalid command\n";
        	int mLength = strlen(message);
        	sys_req(WRITE, DEFAULT_DEVICE, message, &mLength);
        	sys_req(WRITE, DEFAULT_DEVICE, prompt, &pBuffer);
	}
    }
    return 0;
}

int recInput(char* buffer, int* counter) {
    int index = 0;
    int inChar;

    while (index < *counter) {
        if (inb(COM1 + 5) & 1) {

            inChar = inb(COM1);
            //not enter
            if (inChar != 0xD) {
                //backspace
                if (inChar == 0x7f && index > 0) {
                    index--;
                    buffer[index] = '\0';
                    serial_print("\b \b");
                    continue;
                }
                buffer[index] = inChar;
                index++;
                buffer[index] = '\0';
                serial_print(&buffer[index - 1]);
            }
            else {
                buffer[index] = '\0';
                serial_println(" ");
                index = 0;
                //char *inTokens = strtok(buffer, " ");
		char *inTokens = buffer; //rather do this then change all those below
                
                if (isint(inTokens) == 1){
                    return (atoi(inTokens));   
                }
  
                else if (strcmp(inTokens, "yes") == 0) { //for shutdown confirmation, can be used for other things
                    return 104;
                }
                
                else if (strcmp(inTokens, "no") == 0) {// same ^
                    return 105;
                }

                else if (strcmp(inTokens, "shutdown") == 0) {
                    return 106;
                }
                else if (strcmp(inTokens, "help") == 0) {
                    return 107;
                }
                else if (strcmp(inTokens, "version") == 0) {
                    return 108;
                }
                else if (strcmp(inTokens, "get date") == 0) {
                    return 109;
                }
                else if (strcmp(inTokens, "set date") == 0) {
                    return 110;
                }
                else if (strcmp(inTokens, "get time") == 0) {
                    return 111;
                }
                else if (strcmp(inTokens, "set time") == 0) {
                    return 112;
                }
		else if (strcmp(inTokens, "suspend") == 0) {
                    return 113;
                }
		else if (strcmp(inTokens, "resume") == 0) {
                    return 114;
                }
		else if (strcmp(inTokens, "set priority") == 0) {
                    return 115;
                }
		else if (strcmp(inTokens, "show pcb") == 0) {
                    return 116;
                }
		else if (strcmp(inTokens, "show all processes") == 0) {
                    return 117;
                }
		else if (strcmp(inTokens, "show blocked processes") == 0) {
                    return 118;
                }
		else if (strcmp(inTokens, "show ready processes") == 0) {
                    return 119;
                }
		else if (strcmp(inTokens, "create pcb") == 0) {
                    return 120;
                }
		else if (strcmp(inTokens, "delete pcb") == 0) {
                    return 121;
                }
		else if (strcmp(inTokens, "block") == 0) {
                    return 122;
                }
		else if (strcmp(inTokens, "unblock") == 0) {
                    return 123;
                }
		else if (strcmp(inTokens, "yield") == 0) {
		    return 124;
		}
		else if (strcmp(inTokens, "loadr3") == 0) {
		    return 125;
		}
                else {
                    return 666;
                }
            }
        }
    }
    return 0;
}
