#include "test.h"

int main(int argc, char *argv[]){
  coord place;
  char expr0[] = "(#L1) 24";
  char expr1[] = "(#L1 (#S1 #S1)) (1 2 3)";
  char expr2[] = "(#L1 #L2 #S1) 8";
  char expr3[] = "(#L1 (#L2 #S2)) 1 2";
  char expr4[] = "(#L1 (#L2 #S1)) (3 4) 8";

  init(argc, argv);
  place.x = place.y = SIZE/2;

  run_expr(expr0, place);
  SHOULD(count(LAMBDA) == 0);
  SHOULD(count(SYMBOL) == 0);
  SHOULD(count(INTEGER) == 0);

  if(fail_p) ERROR("failed expr0");

  run_expr(expr1, place);
  SHOULD(count(LAMBDA) == 0);
  SHOULD(count(SYMBOL) == 0);
  SHOULD(count(INTEGER) == 6);

  if(fail_p) ERROR("failed expr1");

  run_expr(expr2, place);
  debug(2, "lambda:%d symbol:%d integer:%d\n",
        count(LAMBDA), count(SYMBOL), count(INTEGER));
  SHOULD(count(INTEGER) == 1);
  SHOULD(count(LAMBDA) == 1);
  SHOULD(count(SYMBOL) == 0);

  if(fail_p) ERROR("failed expr2");

  run_expr(expr3, place);
  debug(2, "counting integers\n");
  SHOULD(count(INTEGER) == 1);
  debug(2, "%d integers\n", count(INTEGER));
  debug(2, "counting lambdas\n");
  SHOULD(count(LAMBDA) == 0);
  debug(2, "%d lambdas\n", count(LAMBDA));
  debug(2, "counting symbols\n");
  SHOULD(count(SYMBOL) == 0);
  debug(2, "%d symbols\n", count(SYMBOL));

  if(fail_p) ERROR("failed expr3");

  run_expr(expr4, place);
  place.x = place.y = SIZE/2;
  run_down(place);
  debug(2, "lambda:%d symbol:%d integer:%d\n",
        count(LAMBDA), count(SYMBOL), count(INTEGER));
  SHOULD(count(INTEGER) == 2);
  SHOULD(count(LAMBDA) == 0);
  SHOULD(count(SYMBOL) == 0);

  if(fail_p) ERROR("failed expr4");

  /* return indicates success or failure */
  return fail_p;
}
