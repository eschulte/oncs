#include "test.h"

int main(int argc, char *argv[]){
  init(argc, argv);
  coord place;
  place.x = 4; place.y = 4;

  char expr[] = "(#L0 #S0)((#L0 #S0) (#L1 (#L0 #S0) #S1))";
  string_to_onc(place, expr);
  show_all(place);
  fix(place);
  onc_to_string(place, expr, 0);

  SHOULD(strcmp(expr, "(#L1 (#L0 #S0 ) #S1 )") == 0);
  SHOULD(count(INTEGER) == 0);
  SHOULD(count(LAMBDA) == 2);
  SHOULD(count(SYMBOL) == 2);

  /* return indicates success or failure */
  return fail_p;
}

/*
 * Current
 * -------
 * (#L0 #S0)((#L0 #S0) (#L1 (#L0 #S0) #S1))
 * #S0 -> ((#L0 #S0) (#L1 (#L0 #S0) #S1))
 * #S0 -> (#L1 (#L0 #S0) #S1)
 * #S0 -> (#L1 #S0 -> #S1)
 * (#L1 (#S0))
 */
