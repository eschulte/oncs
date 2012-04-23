#include "test.h"

int main(int argc, char *argv[]){
  init(argc, argv);

  char expr[] =
    "(#L0 (#L1 (#S0 (#L2 ((#S1 #S1) #S2))))"
    "     (#L1 (#S0 (#L2 ((#S1 #S1) #S2)))))"
    "(#L4 #S4)";
  expr_to_expr(expr);
  
  /* SHOULD(count(INTEGER) == 2); */
  /* SHOULD(count(LAMBDA) == 0); */
  /* SHOULD(count(SYMBOL) == 0); */
  /* SHOULD(AT(place).cdr.hdr == INTEGER); */
  /* SHOULD(AT(place).cdr.car == 24); */

  /* return indicates success or failure */
  return fail_p;
}
