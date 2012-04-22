#include "test.h"
/*
 * Should be
 * ---------
 * (#L0 #S0) ((#L0 #S0) (#L1 (#L0 #S0) #S1))
 * (#L0 #S0) (#L1 (#L0 #S0) #S1)
 * (#L1 (#L0 #S0) #S1)
 *
 * because can only evaluate when
 * - outside of a lambda-expression
 * - the argument is a value
 */

void do_run(char *expr);
int main(int argc, char *argv[]){
  init(argc, argv);

  char expr0[] = "(#L0 #S0) (#L1 (#L0 #S0) #S1)";
  do_run(expr0);
  SHOULD(strcmp(expr0, "((((#L1 (#l0 #S0 ) #S1 ) ) ) )"));
  if(fail_p) ERROR("failed expr0");

  char expr1[] = "(#L0 #S0) ((#L0 #S0) (#L1 (#L0 #S0) #S1))";
  do_run(expr1);
  SHOULD(strcmp(expr1, "(((((#L1 (#l0 #S0 ) #S1 ) ) ) ) )") == 0);

  /* TODO: Need to cleanup 2-each extra lambdas and symbols */
  /* debug(2, "%d lambdas\n", count(LAMBDA)); */ /* => 4 */
  /* SHOULD(count(LAMBDA) == 2); */
  /* debug(2, "%d symbols\n", count(SYMBOL)); */ /* => 4 */
  /* SHOULD(count(SYMBOL) == 2); */

  if(fail_p) ERROR("failed expr1");

  char expr2[] = "(#L0 #S0) ((#L0 #S0)(#L1 (#L0 #S0) #S1))";
  do_run(expr2);
  debug(2, "expr2->'%s' => %d\n", expr2);
  SHOULD(strcmp(expr2, "(((((#L1 (#l0 #S0 ) #S1 ) ) ) ) )") == 0);

  if(fail_p) ERROR("failed expr2");

  /* return indicates success or failure */
  return fail_p;
}

void do_run(char *expr){
  coord place;
  place.x = 4; place.y = 4;
  clear_world();
  string_to_onc(place, FALSE, expr);
  show_all(place);
  fix(place);
  onc_to_string(place, expr, 0);
}
