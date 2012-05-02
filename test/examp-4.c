#include "test.h"

int main(int argc, char *argv[]){
  char expr[] =
    "((#L0 #S0)"
    " (((#L3"
    "    ((< 1 #S3)"
    "     89"
    "     ((#l2 (((#l1 ((#l4 (#l3 ((< 1 #S3) 89 (#S4 (- 1 #S3)))))"
    "                   (#l2 ((#S1 #S1) #S2))))"
    "             (#l1 ((#l4 (#l3 ((< 1 #S3) 89 (#S4 (- 1 #S3)))))"
    "                   (#l2 ((#S1 #S1) #S2))))) #S2))"
    "      (- 1 #S3)))))"
    "  0))";

  init(argc, argv);
  expr_to_expr(expr);

  debug(1, "expr:%s\n", expr);
  SHOULD(strcmp(expr, "89") == 0);

  /* return indicates success or failure */
  return fail_p;
}
