#include "test.h"

void full_run(char *expr){
  coord place;
  place.x = place.y = 4;
  clear_world();
  string_to_onc(place, expr, 0);
  show_all(place);
  run_down(place);
  place.x = place.y = 4;
  run_down(place);
  debug(2, "leaving full_run\n");
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
