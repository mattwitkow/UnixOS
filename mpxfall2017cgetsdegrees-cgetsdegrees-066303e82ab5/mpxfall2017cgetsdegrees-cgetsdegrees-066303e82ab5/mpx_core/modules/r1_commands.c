#include "r1_commands.h"
#include <core/serial.h>
#include <core/io.h>
#include <string.h>
#include "mpx_supt.h"
#include "r1_input.h"

int shutdown() {
	char inbuff[100];
	int buff = 100;
	char* confirmPrompt = "Are you sure? 1 for yes or 2 for no\n";
	int pbuff = 50;
	sys_req(WRITE, DEFAULT_DEVICE, confirmPrompt, &pbuff);
	int confirm = sys_req(READ, DEFAULT_DEVICE, inbuff, &buff);
	if(!(confirm == 1 || confirm == 2)) {
		char* message = "must be 1 or 2, shutdown aborted\n";
        	int mLength = strlen(message);
        	sys_req(WRITE, DEFAULT_DEVICE, message, &mLength);
        	return 0;
	}
	if (confirm == 1){
		char* message = "shutdown confirmed\n";
        int mLength = strlen(message);
        sys_req(WRITE, DEFAULT_DEVICE, message, &mLength);
		return -4;
	}
	else {
		char* message = "shutdown cancelled\n";
        int mLength = strlen(message);
        sys_req(WRITE, DEFAULT_DEVICE, message, &mLength);
		return -5;
	}
}

void help() {
	char* message ="help - lists information on available commands\
					\nversion - displays current mpx revision\
					\nshutdown - ends mpx execution\
					\nget time - displays current GMT time\
					\nget date - displays current date\
					\nset time - sets system time\
					\nset date - sets system date\
					\nsuspend - suspends a process\
					\nresume - resumes a process\
					\nset priority - sets pcb priority - prompted for pcb name to set priority of and priority level after entering command\
					\nshow pcb - shows pcb properties - prompted for pcb name to show after entering command\
					\nshow all processes - shows all processes. blocked + ready\
					\nshow ready processes - shows processes in ready queue\
					\nshow blocked processes - shows processes in blocked queue\
					\ncreate pcb - allows for the creation of a pcb - prompted for pcb name to create after entering command\
					\ndelete pcb - allows for the deletion of a pcb - prompted for pcb name to deleat after entering command\
					\nblock - blocks a process - prompted for pcb name to block after entering command\
					\nunblock - unblocks a process - prompted for pcb name to unblock after entering command\
          \nloadr3 - loads processes\
          \nshow allocated mem - shows currently allocated blocks\
          \nshow free mem - shows free blocks\
          \nisempty - is there anything in the heap?\
          \ninit mem - initializes heap - prompts for size\
          \nalloc mem - allocates new block of memory - prompts for size\
          \nfree mem - frees allocated block - prompts for start address of block\n"; 
	int mLength = strlen(message);
	sys_req(WRITE, DEFAULT_DEVICE, message, &mLength);
}

int getTime() {

  int hour, minute, second;
  char bcd_hour[20], bcd_minute[20], bcd_second[20];
  char d_hour[20], d_minute[20], d_second[20];
  char time[50] = "Current Time: ";
  int ibuffer = 50;
	
  outb(0x70, 0x04); //write hours to 0x70 index register
  hour = inb(0x71); //read hours from 0x71 data register
  itoa(hour, bcd_hour, 2);
  btod(bcd_hour, d_hour); //convert bcd to decimal
  strcat(time, d_hour);
  strcat(time, ":");

  outb(0x70, 0x02); //write minutes to 0x70 index register
  minute = inb(0x71); //read minutes from 0x71 data register
  itoa(minute, bcd_minute, 2);
  btod(bcd_minute, d_minute);
  strcat(time, d_minute);
  strcat(time, ":");

  outb(0x70, 0x00); //write seconds to 0x70 index register
  second = inb(0x71); //read seconds from 0x71 data register
  itoa(second, bcd_second, 2);
  btod(bcd_second, d_second);
  strcat(time, d_second);
  strcat(time, " GMT\n");
  sys_req(WRITE, DEFAULT_DEVICE, time, &ibuffer);
	
return 0;

}

int setTime() {

  int h, m, s;
  char cmdBuffer[100];
  char *prompt = "$ ";
  int ibuffer = 50;
  int hflag = 0, mflag = 0, sflag = 0;
  char hMessage[50] = "Please enter the current hour (24-hour clock).\n";
  char mMessage[50] = "Please enter the current minute.\n";
  char sMessage[50] = "Please enter the current second.\n";
  char iMessage[50] = "Please enter a valid time value.\n";
  int setH, setM, setS;

  sys_req(WRITE, DEFAULT_DEVICE, hMessage, &ibuffer);
  sys_req(WRITE, DEFAULT_DEVICE, prompt, &ibuffer);
 

  while (hflag != 1) {
    h = sys_req(READ, DEFAULT_DEVICE, cmdBuffer, &ibuffer); 

    if (h < 0 || h > 23) {
      sys_req(WRITE, DEFAULT_DEVICE, iMessage, &ibuffer);
      sys_req(WRITE, DEFAULT_DEVICE, prompt, &ibuffer);
    } else {
        hflag = 1;
      } 
   }
  
  setH = dtob(h);

  cli(); //disable interrupts
  outb(0x70, 0x04); //write hour to 0x70 index register
  outb(0x71, setH); //write hour to 0x71 data register
  sti(); //enable interrupts

  sys_req(WRITE, DEFAULT_DEVICE, mMessage, &ibuffer);
  sys_req(WRITE, DEFAULT_DEVICE, prompt, &ibuffer);
  
  while (mflag != 1) {
    m = sys_req(READ, DEFAULT_DEVICE, cmdBuffer, &ibuffer);

    if (m < 0 || m > 59) {
      sys_req(WRITE, DEFAULT_DEVICE, iMessage, &ibuffer);
      sys_req(WRITE, DEFAULT_DEVICE, prompt, &ibuffer);
    } else {
        mflag = 1;
      }
  }

  setM = dtob(m);

  cli();
  outb(0x70, 0x02);
  outb(0x71, setM);
  sti();

  sys_req(WRITE, DEFAULT_DEVICE, sMessage, &ibuffer);
  sys_req(WRITE, DEFAULT_DEVICE, prompt, &ibuffer);

  while (sflag != 1) {
    s = sys_req(READ, DEFAULT_DEVICE, cmdBuffer, &ibuffer);

    if (s < 0 || s > 59) {
      sys_req(WRITE, DEFAULT_DEVICE, iMessage, &ibuffer);
      sys_req(WRITE, DEFAULT_DEVICE, prompt, &ibuffer);
    } else {
        sflag = 1;
      }
  }

  setS = dtob(s);

  cli();
  outb(0x70, 0x00);
  outb(0x71, setS);
  sti();

  getTime();
  
return 0;
}

void btod (char* b, char* d){// converts bcd to decimal	
	int len = strlen(b);
	int counter = 0;
	int num = 0;

	if(len>4){
		int j, k;
		for(j = len-4; j>0; j--){
			if(b[j-1] == '1'){
				num = num + pow(2, counter);
			}
			counter=counter+1;
		}
		counter = 0;
		num = num*10;
		
		for(k = len; k > (len-4); k--){
			if(b[k-1] == '1'){
				num = num + pow(2, counter);
			}
			counter=counter+1;
		}
	}

	else{
	int i;
		for(i=len; i>0; i--){
			if(b[i-1] == '1'){
				num = num + pow(2, counter);
			}
			counter=counter+1;
		}
	}

	itoa(num, d, 10);
}

int dtob (int d){

  int result;

  if (d < 9) {
    result = (((d / 10) << 4) | (d % 10));
  } else {
      result = (((d / 100 ) << 8) | ((d / 10) << 4) | (d % 10));
  } 

  return result;

}

//power function for btod, didnt know if we could include math headers
// does a^b
int pow (int a, int b){
	int c = 1;
	int i;
	for(i = 0; i<b; i++){
		c = c * a;
	}
	return c;
}

int version(){
char* v = "Version 5.0     11.10.2017\n";
int buffSize = 30;
sys_req(WRITE, DEFAULT_DEVICE, v, &buffSize);
return 0;
}

int getDate() {

  int dayow, dayom, month, year; //int read from register
  char bcd_dayow[20], bcd_dayom[20], bcd_month[20], bcd_year[20]; //for bcd string of digits
  char d_dayow[20], d_dayom[20], d_month[20], d_year[20]; //for decimal string of digits
  char *e_dayow; //for english string of day of week
  char date[50] = "Current Date: "; //output string
  int ibuffer = 50;

//Day of Week
  outb(0x70, 0x06); //write day of week to 0x70 index register
  dayow = inb(0x71); //read day of week from 0x71 data register
  itoa(dayow, bcd_dayow, 2);
  btod(bcd_dayow, d_dayow);

  if ((strcmp(d_dayow, "1")) == 0) {
    e_dayow = "Sunday";
  } else if ((strcmp(d_dayow, "2")) == 0) {
    e_dayow = "Monday";
  } else if ((strcmp(d_dayow, "3")) == 0) {
    e_dayow = "Tuesday";
  } else if ((strcmp(d_dayow, "4")) == 0) {
    e_dayow = "Wednesday";
  } else if ((strcmp(d_dayow, "5")) == 0) {
    e_dayow = "Thursday";
  } else if ((strcmp(d_dayow, "6")) == 0) { 
    e_dayow = "Friday";
  } else e_dayow = "Saturday";

  strcat(date, e_dayow);
  strcat(date, ", ");

//Month
  outb(0x70, 0x08); //write month to 0x70 index register
  month = inb(0x71); //read month from 0x71 data register
  itoa(month, bcd_month, 2);
  btod(bcd_month, d_month);

  strcat(date, d_month);
  strcat(date, "/");

//Day of Month
  outb(0x70, 0x07); //write day of month to 0x70 index register
  dayom = inb(0x71); //read day of month from 0x71 data register
  itoa(dayom, bcd_dayom, 2);
  btod(bcd_dayom, d_dayom);

  strcat(date, d_dayom);
  strcat(date, "/");

//Year
  outb(0x70, 0x09); //write year to 0x70 index register
  year = inb(0x71); //read year to 0x71 data register
  itoa(year, bcd_year, 2);
  btod(bcd_year, d_year);

  strcat(date, d_year);
  strcat(date, "\n");
  sys_req(WRITE, DEFAULT_DEVICE, date, &ibuffer);  

return 0;
}



int setDate() {

  int d, m, y;
  int dflag = 0, mflag = 0, yflag = 0;
  int setD, setM, setY;
  char cmdBuffer[100];
  int ibuffer = 50;
  char *prompt = "$ ";
  char dMessage[50] = "Please enter the day of the month.\n";
  char mMessage[75] = "Please enter the month (January = 1, December = 12).\n";
  char yMessage[50] = "Please enter the year (YY format).\n";
  char iMessage[50] = "Please enter a valid date value.\n";


  sys_req(WRITE, DEFAULT_DEVICE, mMessage, &ibuffer);
  sys_req(WRITE, DEFAULT_DEVICE, prompt, &ibuffer);

  while (mflag != 1) {

    m = sys_req(READ, DEFAULT_DEVICE, cmdBuffer, &ibuffer);

    if (m < 1 || m > 12){
      sys_req(WRITE, DEFAULT_DEVICE, iMessage, &ibuffer);
      sys_req(WRITE, DEFAULT_DEVICE, prompt, &ibuffer);
    } else {
        mflag = 1;
      }
  }

  setM = dtob(m);

  cli();
  outb(0x70, 0x08);
  outb(0x71, setM);
  sti();
 
  sys_req(WRITE, DEFAULT_DEVICE, yMessage, &ibuffer);
  sys_req(WRITE, DEFAULT_DEVICE, prompt, &ibuffer);

  while (yflag != 1) {

    y = sys_req(READ, DEFAULT_DEVICE, cmdBuffer, &ibuffer);

    if (y < 0 || y > 99){
      sys_req(WRITE, DEFAULT_DEVICE, iMessage, &ibuffer);
      sys_req(WRITE, DEFAULT_DEVICE, prompt, &ibuffer);
    } else {
        yflag = 1;
      }
  }

  setY = dtob(y);

  cli();
  outb(0x70, 0x09);
  outb(0x71, setY);
  sti();

  sys_req(WRITE, DEFAULT_DEVICE, dMessage, &ibuffer);
  sys_req(WRITE, DEFAULT_DEVICE, prompt, &ibuffer);

  while (dflag != 1) {

  d = sys_req(READ, DEFAULT_DEVICE, cmdBuffer, &ibuffer);
  sys_req(WRITE, DEFAULT_DEVICE, prompt, &ibuffer);

  // validity checks for months with 30/31 days and february with leap year checks
    if (d < 1 || d > 31){
      sys_req(WRITE, DEFAULT_DEVICE, iMessage, &ibuffer);
      sys_req(WRITE, DEFAULT_DEVICE, prompt, &ibuffer);
    } else if ((m == 4 || m == 6 || m == 9 || m == 11) && d > 30)  {
      sys_req(WRITE, DEFAULT_DEVICE, iMessage, &ibuffer);
      sys_req(WRITE, DEFAULT_DEVICE, prompt, &ibuffer);
    } else if ((m == 2 && d > 28 && y % 4 != 0)) {
      sys_req(WRITE, DEFAULT_DEVICE, iMessage, &ibuffer); 
      sys_req(WRITE, DEFAULT_DEVICE, prompt, &ibuffer);
    } else {
        dflag = 1;
    }
  }

  setD = dtob(d);

  cli();
  outb(0x70, 0x07);
  outb(0x71, setD);
  sti();

  getDate();  

return 0;
} 


int isint(char* n){
	int len = strlen(n);
	int i;
	int ret = 0;
	if(len > 0){
		for(i = 0; i < len; i++){
			if(n[i] == '0' ||
				n[i] == '1' ||
				n[i] == '2' ||
				n[i] == '3' ||
				n[i] == '4' ||
				n[i] == '5' ||
				n[i] == '6' ||
				n[i] == '7' ||
				n[i] == '8' ||
				n[i] == '9'){
			
			ret = 1;
			}
			else{
				return 0;
			}
		}
	}
	return ret;
}


