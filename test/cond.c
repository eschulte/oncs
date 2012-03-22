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
  coord place;
  place.x = place.y = 4;
  clear_world();
  string_to_onc(place, expr, 0);
  show_all();
  run_down(place);
  place.x = place.y = 4;
  run_down(place);
  debug(2, "leaving full_run\n");
}

void run_down(coord place){
  run(place); show_all();
  while(queue_population() > 0){
    place = queue[qbeg].coord;
    debug(2, "run queue to (%d,%d)\n", place.x, place.y);
    run_queue(); show_all();
    debug(2, "run (%d,%d)\n", place.x, place.y);
    run(place);  show_all();
    debug(2, "checking queue\n");
  };
  debug(2, "leaving run_down\n");
}

int main(int argc, char *argv[]){
  init(argc, argv);

  char expr0[] = "(#L1) (24)";
  full_run(expr0);
  SHOULD(count(LAMBDA) == 0);
  SHOULD(count(SYMBOL) == 0);
  SHOULD(count(INTEGER) == 0);

  if(fail_p) ERROR("failed expr0");

  char expr1[] = "(#L1 (#L2 (#S1))) (8)";
  full_run(expr1);
  SHOULD(count(LAMBDA) == 1);
  SHOULD(count(SYMBOL) == 0);
  SHOULD(count(INTEGER) == 1);

  if(fail_p) ERROR("failed expr0");

  char expr2[] =
    "((#L1 (#L2 (#S2))) (1 (2 3))) ((#L1 (#L2 (#S1))) (3 4))";
  full_run(expr2);
  debug(2, "counting integers\n");
  SHOULD(count(INTEGER) == 2);
  debug(2, "%d integers\n", count(INTEGER));
  debug(2, "counting lambdas\n");
  SHOULD(count(LAMBDA) == 1);
  debug(2, "%d lambdas\n", count(LAMBDA));
  debug(2, "counting symbols\n");
  SHOULD(count(SYMBOL) == 0);
  debug(2, "%d symbols\n", count(SYMBOL));

  if(fail_p) ERROR("failed expr0");

  /* return indicates success or failure */
  return fail_p;
}
