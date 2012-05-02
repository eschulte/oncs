#include "test.h"

int main(int argc, char *argv[]){
  char expr[] =
    "(#L1 ((= #S1 0)"
    "       1"
    "       (* #S1"
    "          ((#L2 ((= #S2 0)"
    "                1"
    "                (* #S2 ((#L3 ((= #S3 0)"
    "                         1 #S99))"
    "                    (- 1 #S2)))))"
    "           (- 1 #S1)))))"
    "2";

  init(argc, argv);
  expr_to_expr(expr);
  SHOULD(strcmp(expr, "2") == 0);

  /* return indicates success or failure */
  return fail_p;
}
