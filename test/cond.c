#include "test.h"

int main(int argc, char *argv[]){
  init(argc, argv);
  coord place;
  place.x = 4; place.y = 4;

  /* char expr0[] = "(#L1) 24"; */
  /* run_expr(expr0, place); */
  /* SHOULD(count(LAMBDA) == 0); */
  /* SHOULD(count(SYMBOL) == 0); */
  /* SHOULD(count(INTEGER) == 0); */

  /* if(fail_p) ERROR("failed expr0"); */

  /* char expr1[] = "(#L1 (#S1 #S1)) (1 2 3)"; */
  /* run_expr(expr1, place); */
  /* SHOULD(count(LAMBDA) == 0); */
  /* SHOULD(count(SYMBOL) == 0); */
  /* SHOULD(count(INTEGER) == 3); */

  /* if(fail_p) ERROR("failed expr1"); */

  /* char expr2[] = "(#L1 #L2 #S1) 8"; */
  /* run_expr(expr2, place); */
  /* SHOULD(count(LAMBDA) == 1); */
  /* SHOULD(count(SYMBOL) == 0); */
  /* SHOULD(count(INTEGER) == 1); */

  /* if(fail_p) ERROR("failed expr2"); */

  /* char expr3[] = "(#L1 #L2 #S2) 1 2"; */
  /* run_expr(expr3, place); */
  /* debug(2, "counting integers\n"); */
  /* SHOULD(count(INTEGER) == 1); */
  /* debug(2, "%d integers\n", count(INTEGER)); */
  /* debug(2, "counting lambdas\n"); */
  /* SHOULD(count(LAMBDA) == 0); */
  /* debug(2, "%d lambdas\n", count(LAMBDA)); */
  /* debug(2, "counting symbols\n"); */
  /* SHOULD(count(SYMBOL) == 0); */
  /* debug(2, "%d symbols\n", count(SYMBOL)); */

  /* if(fail_p) ERROR("failed expr3"); */

  /* char expr4[] = "(#L1 #L2 #S2) (1 2 3) ((#L1 #L2 #S1) (3 4) 8)"; */
  /* run_expr(expr4, place); */
  /* place.x = place.y = 4; */
  /* run_down(place); */
  /* debug(2, "counting integers\n"); */
  /* SHOULD(count(INTEGER) == 2); */
  /* debug(2, "%d integers\n", count(INTEGER)); */
  /* debug(2, "counting lambdas\n"); */
  /* SHOULD(count(LAMBDA) == 0); */
  /* debug(2, "%d lambdas\n", count(LAMBDA)); */
  /* debug(2, "counting symbols\n"); */
  /* SHOULD(count(SYMBOL) == 0); */
  /* debug(2, "%d symbols\n", count(SYMBOL)); */

  /* if(fail_p) ERROR("failed expr4"); */

  /* return indicates success or failure */
  return fail_p;
}
