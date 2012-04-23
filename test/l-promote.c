#include "test.h"

int main(int argc, char *argv[]){
  init(argc, argv);
  char expr[] = "(#L1 #S1) 1 2";
  expr_to_expr(expr);

  SHOULD(strcmp(expr, "1 2") == 0);
  SHOULD(count(INTEGER) == 2);
  SHOULD(count(LAMBDA) == 0);
  SHOULD(count(SYMBOL) == 0);
  SHOULD(count(LOCAL) == 1);
  SHOULD(count(NIL) == 1);

  /* return indicates success or failure */
  return fail_p;
}
