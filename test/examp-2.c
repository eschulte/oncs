#include "test.h"

int main(int argc, char *argv[]){
  char expr[] = "(#L1 (#S0 #S1)) (78)";

  init(argc, argv);
  expr_to_expr(expr);

  debug(1, "expr:%s\n", expr);
  SHOULD(strcmp(expr, "(#S0 78)") == 0);
  
  SHOULD(count(INTEGER) == 1);
  SHOULD(count(LAMBDA) == 0);
  SHOULD(count(SYMBOL) == 1);

  /* return indicates success or failure */
  return fail_p;
}

/* Possible Examples
 * -----------------
 *
 * (#L1 (#S0 #S1)) (#L1 (#S0 #S1))
 * 
 */
