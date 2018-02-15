//header file for module 1 commands
#ifndef R1_COMMANDS_H
#define R1_COMMANDS_H
/**
  shutdown displays a prompt asking for confirmation that the user would like to shut down
*/

int shutdown();
/**
  help lists the commands available to the user
*/

void help();
/**
  getTime displays the current system time
*/

int getTime();
/**
  btod converts bcd number to decimal using pointers
*/

void btod(char* b, char* d);
/**
  dtob converts decimal numbers to bcd
*/

int dtob(int d);
/**
  pow raises input a to the power of input b and returns that integer value
*/

int pow(int a, int b);
/**
  version displays the current software iteration with its release date
*/

int version();
/**
  getDate displays the current date stored by the system
*/

int getDate();

/**
  setTime sets the system time in hours, minutes, seconds and displays it
*/

int setTime();
/**
  setDate sets the system date month, day, year and displays it
*/
int setDate();
/**
  isint checks to see if a string input is an integer number
*/
int isint(char* n);

#endif
