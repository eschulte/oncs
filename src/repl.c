#include "test.h"
#define MAX_INPUT_LENGTH 1000
#define OTHER   0
#define QUIT    1
#define FIX     2
#define SHOW    3
#define CLEAR   4
#define VERBOSE 5

int code(char *string);

int main(int argc, char *argv[]){
  int mid=SIZE/2;
  coord place;
  place.x = mid; place.y = mid;
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

      /* REPL */
      switch(code(input)){
      case QUIT:
        running=0;
        break;
      case FIX:
        fix(place);
        onc_to_string(place, input, 0);
        printf("%s\n", input);
        break;
      case SHOW:
        verbose=1;
        show_world();
        verbose=0;
        break;
      case CLEAR:
        clear_world();
        break;
      case VERBOSE:
        if(verbose == 1) verbose=0;
        else             verbose=1;
        printf("verbose=%d\n", verbose);
        break;
      case OTHER:
        run_string(input);
        printf("%s\n", input);
        break;
      }
    }
  } while (running);
  printf("bye\n");
  return 0;
}

static struct lookup_table { char *string; int code;
} codes[] = {
  {"quit", QUIT}, {"Quit", QUIT}, {"QUIT", QUIT},
  {"fix", FIX}, {"Fix", FIX}, {"FIX", FIX},
  {"show", SHOW}, {"Show", SHOW}, {"SHOW", SHOW},
  {"clear", CLEAR}, {"Clear", CLEAR}, {"CLEAR", CLEAR},
  {"verbose", VERBOSE}, {"Verbose", VERBOSE}, {"VERBOSE", VERBOSE}
};

int code(char *string){
  int i;
  for(i = 0; i < sizeof(codes) / sizeof(codes[0]); i++)
    if(strcmp(codes[i].string, string) == 0)
      return codes[i].code;
  return OTHER;
}
