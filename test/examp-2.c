#include "test.h"

int main(int argc, char *argv[]){
  init(argc, argv);
  coord place;
  place.x = 4; place.y = 4;

  /* TODO: ensure the 78 never has a ref count less than 1 */
  char expr0[] = "(#L1 (#S0 #S1)) 78";
  string_to_onc(place, FALSE, expr0);
  show_all(place);
  fix(place);

  SHOULD(strcmp(expr0, "(#S0 78)") == 0);

  /* SHOULD(count(INTEGER) == 2); */
  /* SHOULD(count(LAMBDA) == 0); */
  /* SHOULD(count(SYMBOL) == 0); */
  /* SHOULD(AT(place).cdr.hdr == INTEGER); */
  /* SHOULD(AT(place).cdr.car == 24); */

  /* TODO: ensure the (78) never has a ref count less than 1 */
  char expr1[] = "(#L1 (#S0 #S1)) (78)";
  string_to_onc(place, FALSE, expr1);
  show_all(place);
  fix(place);

  SHOULD(strcmp(expr1, "(#S0 78)") == 0);


  /* return indicates success or failure */
  return fail_p;
}

/* Possible Examples
 * -----------------
 *
 * (#L1 (#S0 #S1)) (#L1 (#S0 #S1))
 * 
 */
