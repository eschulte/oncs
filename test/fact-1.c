#include "test.h"

int main(int argc, char *argv[]){
  char expr[] = "(#L1 ((= #S1 0) 1 (* #S1 (#S2 - 1 #S1)))) 1";

  init(argc, argv);
  expr_to_expr(expr);
  SHOULD(strcmp(expr, "(*1 (#S2 0))") == 0);

  /* return indicates success or failure */
  return fail_p;
}
