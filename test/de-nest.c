#include "test.h"

int main(int argc, char *argv[]){
  char expr[] = "((1))";
  char expr1[] = "((1)) 1";
  char expr2[] = "((#L1 #S1)) 2";

  init(argc, argv);

  expr_to_expr(expr);
  SHOULD(strcmp(expr, "1") == 0);
  if(fail_p) ERROR("failed expr");

  expr_to_expr(expr1);
  SHOULD(strcmp(expr1, "1 1") == 0);
  if(fail_p) ERROR("failed expr1");

  expr_to_expr(expr2);
  SHOULD(strcmp(expr2, "2") == 0);
  if(fail_p) ERROR("failed expr2");

  /* return indicates success or failure */
  return fail_p;
}
