#include "test.h"

void single_int_left();

int main(int argc, char *argv[]){
  init(argc, argv);
  coord place;
  place.x = 4; place.y = 4;

  char expr0[] = "+ 2 4";
  run_expr(expr0, place);
  run_down(place);
  SHOULD(AT(place).car.hdr == INTEGER);
  SHOULD(AT(place).car.car == 6);
  SHOULD(AT(place).cdr.hdr == NIL);
  single_int_left();
  if(fail_p) ERROR("failed expr0");

  char expr1[] = "/ 8 2";
  run_expr(expr1, place);
  run_down(place);
  single_int_left();
  if(fail_p) ERROR("failed expr1");

  char expr2[] = "* + 1 2 - 8 32";
  run_expr(expr2, place);
  fix(place);
  SHOULD(AT(place).car.hdr == INTEGER);
  SHOULD(AT(place).car.car == 72);
  single_int_left();
  if(fail_p) ERROR("failed expr2");

  /* return indicates success or failure */
  return fail_p;
}

void single_int_left(){
  SHOULD(count(INTEGER) == 1);
  debug(2, "%d integers failed?:%d\n", count(INTEGER), fail_p);
  SHOULD(count(LAMBDA) == 0);
  debug(2, "%d lambdas failed?:%d\n", count(LAMBDA), fail_p);
  SHOULD(count(LOCAL) == 0);
  debug(2, "%d locals failed?:%d\n", count(LOCAL), fail_p);
  SHOULD(count(NIL) == 1);
  debug(2, "%d nils failed?:%d\n", count(NIL), fail_p);
}
