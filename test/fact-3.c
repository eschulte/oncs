#include "test.h"

int main(int argc, char *argv[]){
  init(argc, argv);

  char expr[] =
    "(#L1 (= #S1 0 1 (* #S1 ((#L0 (= #S0 0 1 (* #S0 (- 1 #S0)))) - 1 #S1))))"
    "2";
  expr_to_expr(expr);
  SHOULD(strcmp(expr, "2") == 0);

  /* return indicates success or failure */
  return fail_p;
}
