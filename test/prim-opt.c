#include "test.h"

int main(int argc, char *argv[]){
  init(argc, argv);
  coord place;
  place.x = 4; place.y = 4;

  char expr0[] = "+ (2 (4))";
  run_expr(expr0, place);
  run_down(place);
  SHOULD(AT(place).car.hdr == INTEGER);
  SHOULD(AT(place).car.car == 6);
  SHOULD(count(INTEGER) == 1);
  SHOULD(count(LAMBDA) == 0);
  SHOULD(count(LOCAL) == 0);

  if(fail_p) ERROR("failed expr0");

  char expr1[] = "* ((+ (1 (2))) ((- (32 (8)))))";
  run_expr(expr1, place);
  run_down(place);
  run_down(place);
  SHOULD(AT(place).car.hdr == INTEGER);
  SHOULD(AT(place).car.car == 72);

  /* if(fail_p) ERROR("failed expr0"); */

  /* char expr2[] = "";*/
  /* run_expr(expr2, place); */
  /* debug(2, "counting integers\n"); */
  /* SHOULD(count(INTEGER) == 2); */
  /* debug(2, "%d integers\n", count(INTEGER)); */
  /* debug(2, "counting lambdas\n"); */
  /* SHOULD(count(LAMBDA) == 1); */
  /* debug(2, "%d lambdas\n", count(LAMBDA)); */
  /* debug(2, "counting symbols\n"); */
  /* SHOULD(count(SYMBOL) == 0); */
  /* debug(2, "%d symbols\n", count(SYMBOL)); */

  /* if(fail_p) ERROR("failed expr0"); */

  /* return indicates success or failure */
  return fail_p;
}
