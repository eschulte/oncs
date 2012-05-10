#include "test.h"

int main(int argc, char *argv[]){
  coord place;  
  char expr0[] = "= 2 2";
  char expr1[] = "= 2 3";
  char expr2[] = "< 2 3";
  char expr3[] = "< 3 3";
  char expr4[] = "= 0 1 1 2";
  char expr5[] = "= 1 1 1 2";
  char expr6[] = "((= 1 1) 1 2)";

  init(argc, argv);
  place.x = place.y = SIZE/2;

  run_expr(expr0, place);
  run_down(place);
  SHOULD(count(LAMBDA) == 2);
  SHOULD(count(SYMBOL) == 1);

  if(fail_p) ERROR("failed expr0");

  run_expr(expr1, place);
  run_down(place);
  SHOULD(count(LAMBDA) == 2);
  SHOULD(count(SYMBOL) == 1);

  if(fail_p) ERROR("failed expr1");

  run_expr(expr2, place);
  run_down(place);
  SHOULD(count(LAMBDA) == 2);
  SHOULD(count(SYMBOL) == 1);

  if(fail_p) ERROR("failed expr2");

  run_expr(expr3, place);
  run_down(place);
  SHOULD(count(LAMBDA) == 2);
  SHOULD(count(SYMBOL) == 1);

  if(fail_p) ERROR("failed expr3");

  expr_to_expr(expr4);
  debug(2, "lambda:%d symbol:%d integer:%d\n",
        count(LAMBDA), count(SYMBOL), count(INTEGER));
  SHOULD(count(LAMBDA) == 0);
  SHOULD(count(SYMBOL) == 0);
  SHOULD(count(INTEGER) == 1);

  if(fail_p) ERROR("failed expr4");

  expr_to_expr(expr5);
  debug(2, "lambda:%d symbol:%d integer:%d\n",
        count(LAMBDA), count(SYMBOL), count(INTEGER));
  SHOULD(count(LAMBDA) == 0);
  SHOULD(count(SYMBOL) == 0);
  SHOULD(count(INTEGER) == 1);

  if(fail_p) ERROR("failed expr5");

  expr_to_expr(expr6);
  debug(2, "lambda:%d symbol:%d integer:%d\n",
        count(LAMBDA), count(SYMBOL), count(INTEGER));
  SHOULD(count(LAMBDA) == 0);
  SHOULD(count(SYMBOL) == 0);
  SHOULD(count(INTEGER) == 1);

  if(fail_p) ERROR("failed expr6");

  /* return indicates success or failure */
  return fail_p;
}
