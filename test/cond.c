#include "test.h"
#define BUF_SIZE 1024

void show_all(){
  coord place;
  place.x = place.y = 4;
  char buf[BUF_SIZE];
  show_world();
  onc_to_string(place, buf, 0);
  debug(1, "expr(%d,%d):%s\n", place.x, place.y, buf);
}

void full_run(char *expr){
  debug(2, "1\n");
  debug(1, "\nrunning: %s\n", expr);
  coord place;
  place.x = place.y = 4;
  clear_world();
  run_queue(); debug(2, "4\n"); show_all(); debug(2, "5\n");
  string_to_onc(place, expr, 0);
  show_all();
  run_queue(); debug(2, "4\n"); show_all(); debug(2, "5\n");
  run(place);
  show_all();
  run_queue(); debug(2, "4\n"); show_all(); debug(2, "5\n");
  debug(2, "2\n");
  run_down();
  place.x = place.y = 4;
  run_down();
  show_all();
}

void run_down(){
  coord place;
  do{
    place = queue[qbeg].coord; debug(2, "3\n");
    if(queue_population() > 0){
      run_queue(); show_all();
      run(place);  show_all();
    }
  } while(queue_population() > 0);
}

int main(int argc, char *argv[]){
  init(argc, argv);

  /* char expr0[] = "(#L1) 24"; */
  /* full_run(expr0); */

  /* char expr1[] = "(#L1 (#L2 (#S1))) (8)"; */
  /* full_run(expr1); */
  /* SHOULD(count(LAMBDA) == 1); */
  /* SHOULD(count(SYMBOL) == 0); */
  /* SHOULD(count(INTEGER) == 1); */

  char expr2[] =
    "((#L1 (#L2 #S2)) (1 (2 3))) ((#L1 (#L2 #S1)) (3 4))";
  full_run(expr2);
  full_run(expr2);
  SHOULD(count(INTEGER) == 2);
  SHOULD(count(LAMBDA) == 0);
  SHOULD(count(SYMBOL) == 0);

  /* return indicates success or failure */
  return fail_p;
}
