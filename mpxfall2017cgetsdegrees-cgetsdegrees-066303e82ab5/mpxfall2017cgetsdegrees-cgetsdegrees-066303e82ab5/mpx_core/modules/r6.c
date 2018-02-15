#include <stdio.h>
#include <string.h>

int main(void) {
  char in[20];
  printf("MODULE R6 -- File System\n");
  int flag = 1;
  while(flag) {
    printf(":> ");
    fgets(in, 20, stdin);
    if(strcmp(in, "quit") == 10) {  //idk why this == 10 and not 0 im dumb
      flag = 0;
    }
  }
  printf("goodbye\n");
  return 0;
}
