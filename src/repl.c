#include "test.h"
#include <readline/readline.h>
#include <readline/history.h>

/* REPL commands */
#define OTHER   0
#define QUIT    1
#define FIX     2
#define SHOW    3
#define CLEAR   4
#define VERBOSE 5
#define HELP    6
#define STEP    7
static struct lookup_table { char *string; int code; char *doc;
} codes[] = {
  {"quit", QUIT, "quit the repl"},
  {"fix", FIX, "run until reaching a fixed point"},
  {"show", SHOW, "show the contents of the ONC world"},
  {"clear", CLEAR, "clear the ONC world"},
  {"verbose", VERBOSE, "toggle verbose execution"},
  {"help", HELP, "show this help message"},
  {"step", STEP, "run a single step"}
};

int code(char *string);

int main(int argc, char *argv[]){
  int mid=SIZE/2;
  coord place;
  place.x = mid; place.y = mid;
  char* input;
  static char* shell_prompt = "> ";
  int running=1;
  int i;
  do{
    input = readline(shell_prompt);
    if(input) add_history(input);
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
    case HELP:
      for(i = 0; i < sizeof(codes) / sizeof(codes[0]); i++)
        printf("%8s -- %s\n", codes[i].string, codes[i].doc);
      printf("\n"
             "All other inputs should be lambda calculus\n"
             "expressions.  Allowed atoms include...\n"
             "#Ln -- λn, `n' is an integer symbol identifier\n"
             "#Sn -- symbol n, `n' is an integer symbol identifier\n"
             "  n -- literal integer `n'\n"
             " op -- where `op' is a primitive integer operation\n"
             "       and op is one of (+ - * / = <)\n");
      break;
    case STEP:
      step(place);
      onc_to_string(place, input, 0);
      printf("%s\n", input);
      break;
    case OTHER:
      string_to_onc(place, FALSE, input);
      onc_to_string(place, input, 0);
      printf("%s\n", input);
      break;
    }
  } while (running);
  printf("bye\n");
  return 0;
}

int code(char *string){
  int i;
  if(!string) return QUIT;
  for(i = 0; i < sizeof(codes) / sizeof(codes[0]); i++)
    if(strcmp(codes[i].string, string) == 0)
      return codes[i].code;
  return OTHER;
}
