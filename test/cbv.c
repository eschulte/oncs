#include "test.h"
/*
 * Should be
 * ---------
 * (#L0 #S0) ((#L0 #S0) (#L1 (#L0 #S0) #S1))
 * (#L0 #S0) (#L1 (#L0 #S0) #S1)
 * (#L1 (#L0 #S0) #S1)
 *
 * because can only evaluate when
 * - outside of a lambda-expression
 * - the argument is a value
 */

int main(int argc, char *argv[]){
  char expr[] = "(#L1 (#L0 #S0) #S1)";
  char expr0[] = "(#L0 #S0) (#L1 (#L0 #S0) #S1)";
  char expr1[] = "(#L0 #S0) ((#L0 #S0) (#L1 (#L0 #S0) #S1))";

  init(argc, argv);

  expr_to_expr(expr);
  SHOULD(strcmp(expr, "(#L1 (#l0 #S0) #S1)") == 0);
  if(fail_p) ERROR("failed expr");

  expr_to_expr(expr0);
  SHOULD(strcmp(expr0, "(#L1 (#l0 #S0) #S1)") == 0);
  if(fail_p) ERROR("failed expr0");

  expr_to_expr(expr1);
  SHOULD(strcmp(expr1, "(#L1 (#l0 #S0) #S1)") == 0);

  debug(2, "%d lambdas\n", count(LAMBDA));
  SHOULD(count(LAMBDA) == 2);
  debug(2, "%d symbols\n", count(SYMBOL));
  SHOULD(count(SYMBOL) == 2);

  if(fail_p) ERROR("failed expr1");

  /* return indicates success or failure */
  return fail_p;
}
