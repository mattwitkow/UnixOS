//header file for our input function
#ifndef R1_INPUT_H
#define R1_INPUT_H
/**
  @brief comHand is responsible for determining which command the user inputted based on the output of recInput and reacting accordingly
*/
int comHand(int insideCommand);
/**
  @brief recInput accumulates a buffer of characters and returns integer values based upon which command was entered by the user. Also accounts for use of backspace and error keys
*/

int recInput(char* buffer, int* count);


#endif
