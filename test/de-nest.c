#include "test.h"

int main(int argc, char *argv[]){
  init(argc, argv);

  char expr[] = "((1))";
  expr_to_expr(expr);
  SHOULD(strcmp(expr, "(1)") == 0);

  char expr1[] = "((1) 1)";
  expr_to_expr(expr1);
  SHOULD(strcmp(expr1, "((1) 1)") == 0);

  /* return indicates success or failure */
  return fail_p;
}
