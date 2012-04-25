#include "test.h"

int main(int argc, char *argv[]){
  init(argc, argv);

  char expr[] = "(#L1 (= #S1 0 1 (* #S1 (#S2 - 1 #S1)))) 0";
  expr_to_expr(expr);
  SHOULD(strcmp(expr, "1") == 0);

  /* return indicates success or failure */
  return fail_p;
}
