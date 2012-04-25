#include "test.h"

int main(int argc, char *argv[]){
  init(argc, argv);

  char expr[] =
    "(#L1 ((= #S1 0)"
    "       1"
    "       (* #S1"
    "          ((#L2 ((= #S2 0)"
    "                1"
    "                (* #S2 ((#L3 ((= #S3 0)"
    "                          1"
    "                          (* #S3 ((#L4 ((= #S4 0)"
    "                                   1"
    "                                   (* #S4 #S99)))"
    "                             (- 1 #S3)))))"
    "                    (- 1 #S2)))))"
    "           (- 1 #S1)))))"
    "3";
  expr_to_expr(expr);
  SHOULD(strcmp(expr, "6") == 0);

  /* return indicates success or failure */
  return fail_p;
}
