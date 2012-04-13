#include "test.h"
#define MAX_INPUT_LENGTH 10000

char *newline;
char input[MAX_INPUT_LENGTH];

void process(char* expr){
  int mid=SIZE/2;
  coord place;
  place.x = mid; place.y = mid;
  clear_world();
  run_expr(expr, place);
  fix(place);
  onc_to_string(place, input, 0);
  printf("%s\n", input);
}

int main(int argc, char *argv[]){
  int i=0;
  if(argc > 1){
    for(i=1;i<argc;i++)
      process(argv[i]);
  } else {
    while(fgets(input, sizeof input, stdin) != NULL){
      /* search for and replace newline character */
      newline = strchr(input, '\n');
      if(newline != NULL) *newline = '\0';
      /* Run the string */
      process(input);
    }
  }
  return 0;
}
