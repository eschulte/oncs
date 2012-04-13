#include "test.h"
#define MAX_INPUT_LENGTH 1000

int main(int argc, char *argv[]){
  char *newline;
  char input[MAX_INPUT_LENGTH];
  int running=1;
  do{
    fputs("> ", stdout);
    fflush(stdout);
    if(fgets(input, sizeof input, stdin) == NULL) {
      running=0;
    } else {
      /* search for and replace newline character */
      newline = strchr(input, '\n');
      if(newline != NULL) *newline = '\0';
      if(strcmp(input, "quit") == 0){
        printf("bye\n");
        running=0;
      } else {
        printf("%s\n", run_string(input));
      }
    }
  } while (running);
  return 0;
}
