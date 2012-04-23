#include "test.h"

int main(int argc, char *argv[]){
  init(argc, argv);

  /* TODO: ensure the (78) never has a ref count less than 1 */
  char expr[] = "(#L1 (#S0 #S1)) (78)";
  expr_to_expr(expr);

  debug(1, "expr:%s\n", expr);
  SHOULD(strcmp(expr, "(#S0 (78))") == 0);
  
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
