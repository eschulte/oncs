#include "test.h"

int main(int argc, char *argv[]){
  char expr[] =
    "((#L1 (#L0 #S0))"
    " 1"
    " (* 3 ((#L0 #S0)"
    "       (* 2 ((#L1 (#L0 #S0))"
    "             1"
    "             (* 1 ((=0 0) 1 (*0 #S99))))))))";

  init(argc, argv);
  expr_to_expr(expr);
  debug(1, "expr:%s\n", expr);
  SHOULD(strcmp(expr, "6") == 0);

  /* return indicates success or failure */
  return fail_p;
}
