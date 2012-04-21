#include "test.h"

int main(int argc, char *argv[]){
  init(argc, argv);
  coord place;
  place.x = 4; place.y = 4;

  char expr[] = "(#L1 #S1 #S1) 1";
  string_to_onc(place, FALSE, expr);
  show_all(place);
  fix(place);
  onc_to_string(place, expr, 0);

  SHOULD(strcmp(expr, "(1 1 )") == 0);
  SHOULD(count(INTEGER) == 2);
  SHOULD(count(LAMBDA) == 0);
  SHOULD(count(SYMBOL) == 0);
  SHOULD(count(LOCAL) == 2);
  SHOULD(count(NIL) == 1);

  /* return indicates success or failure */
  return fail_p;
}
