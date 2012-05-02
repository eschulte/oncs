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
  char expr[] = "(#L1 (#L2 (#L3 #S1))) (#L4 #S4)";

  init(argc, argv);
  expr_to_expr(expr);
  SHOULD(strcmp(expr, "(#L2 (#l3 (#l4 #S4)))") == 0);

  /* return indicates success or failure */
  return fail_p;
}
