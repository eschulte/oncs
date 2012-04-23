#include "test.h"

int main(int argc, char *argv[]){
  init(argc, argv);

  char expr_true[] = "(#L0 (#L1 #S0)) 1 2";
  expr_to_expr(expr_true);

  debug(1, "expr_true:%s\n", expr_true);
  SHOULD(strcmp(expr_true, "1") == 0);
  
  SHOULD(count(INTEGER) == 1);
  SHOULD(count(LAMBDA) == 0);
  SHOULD(count(SYMBOL) == 0);

  if(fail_p) ERROR("failed expr_true");

  char expr_false[] = "(#L0 (#L1 #S1)) 1 2";
  expr_to_expr(expr_false);

  debug(1, "expr_false:%s\n", expr_false);
  SHOULD(strcmp(expr_false, "2") == 0);
  
  SHOULD(count(INTEGER) == 1);
  SHOULD(count(LAMBDA) == 0);
  SHOULD(count(SYMBOL) == 0);

  if(fail_p) ERROR("failed expr_false");

  /* return indicates success or failure */
  return fail_p;
}
