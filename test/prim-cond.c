#include "test.h"

int main(int argc, char *argv[]){
  init(argc, argv);
  coord place;
  place.x = 4; place.y = 4;

  char expr0[] = "(= 2 2)";
  run_expr(expr0, place);
  run_down(place);
  SHOULD(count(LAMBDA) == 2);
  SHOULD(count(SYMBOL) == 1);

  if(fail_p) ERROR("failed expr0");

  char expr1[] = "(= 2 3)";
  run_expr(expr1, place);
  run_down(place);
  SHOULD(count(LAMBDA) == 2);
  SHOULD(count(SYMBOL) == 1);

  if(fail_p) ERROR("failed expr1");

  char expr2[] = "(< 2 3)";
  run_expr(expr2, place);
  run_down(place);
  SHOULD(count(LAMBDA) == 2);
  SHOULD(count(SYMBOL) == 1);

  if(fail_p) ERROR("failed expr2");

  char expr3[] = "(< 3 3)";
  run_expr(expr3, place);
  run_down(place);
  SHOULD(count(LAMBDA) == 2);
  SHOULD(count(SYMBOL) == 1);

  if(fail_p) ERROR("failed expr3");

  char expr4[] = "((= 0 1) #S1 #S2)";
  run_expr(expr4, place);
  fix(place);
  SHOULD(count(LAMBDA) == 0);
  SHOULD(count(SYMBOL) == 1);
  SHOULD(count(INTEGER) == 0);

  if(fail_p) ERROR("failed expr4");

  /* return indicates success or failure */
  return fail_p;
}
