#include "test.h"

int main(int argc, char *argv[]){
  init(argc, argv);

  char expr[] = "(#L3 (= 0 #S3 1 (* #S3 (#S2 - #S3 1)))) 1";
  expr_to_expr(expr);
  SHOULD(strcmp(expr, "(*1 (#S2 0)))") == 0);

  /* return indicates success or failure */
  return fail_p;
}
