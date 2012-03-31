#include "test.h"

int main(int argc, char *argv[]){
  init(argc, argv);
  coord place;
  place.x = 4; place.y = 4;

  char expr0[] = "= (2 (2))";
  run_expr(expr0, place);
  run_down(place);
  SHOULD(count(BOOLEAN) == 1);
  SHOULD(count(UNPACK) == 1);
  SHOULD(AT(place).cdr.car == TRUE);

  if(fail_p) ERROR("failed expr0");

  char expr1[] = "= (2 (3))";
  run_expr(expr1, place);
  run_down(place);
  SHOULD(count(BOOLEAN) == 1);
  SHOULD(count(UNPACK) == 1);
  SHOULD(AT(place).cdr.car == FALSE);

  if(fail_p) ERROR("failed expr1");

  char expr2[] = "< (2 (3))";
  run_expr(expr2, place);
  run_down(place);
  SHOULD(count(BOOLEAN) == 1);
  SHOULD(count(UNPACK) == 1);
  SHOULD(AT(place).cdr.car == TRUE);

  if(fail_p) ERROR("failed expr2");

  char expr3[] = "< (3 (3))";
  run_expr(expr3, place);
  run_down(place);
  SHOULD(count(BOOLEAN) == 1);
  SHOULD(count(UNPACK) == 1);
  SHOULD(AT(place).cdr.car == FALSE);

  if(fail_p) ERROR("failed expr3");

  /* return indicates success or failure */
  return fail_p;
}
