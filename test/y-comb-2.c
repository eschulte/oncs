#include "test.h"

int main(int argc, char *argv[]){
  char expr[] =
    "((#L0 (#L1 (#S0 (#L2 ((#S1 #S1) #S2))))"
    "      (#L1 (#S0 (#L2 ((#S1 #S1) #S2)))))"
    " (#L4 (#L3 ((< 1 #S3) 89 (#S4 (- 1 #S3))))))"
    "1";

  init(argc, argv);
  expr_to_expr(expr);
  SHOULD(strcmp(expr, "89") == 0);

  /* return indicates success or failure */
  return fail_p;
}
