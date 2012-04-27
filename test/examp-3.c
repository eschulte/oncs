#include "test.h"

int main(int argc, char *argv[]){
  init(argc, argv);

  char expr[] =
    "((#L1 (#L0 #S0))"
    " 1"
    " (* 3 ((#L0 #S0)"
    "       (* 2 ((#L1 (#L0 #S0))"
    "             1"
    "             (* 1 ((=0 0) 1 (*0 #S99))))))))";
  expr_to_expr(expr);

  debug(1, "expr:%s\n", expr);
  SHOULD(strcmp(expr, "6") == 0);

  /* return indicates success or failure */
  return fail_p;
}
