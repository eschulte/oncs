#include "test.h"

int main(int argc, char *argv[]){
  init(argc, argv);
  coord place;
  place.x = 4; place.y = 4;

  char expr0[] = "+ (2 (4))";
  run_expr(expr0, place);
  run_down(place);
  SHOULD(AT(place).car.hdr == UNPACK);
  SHOULD(AT(place).cdr.hdr == INTEGER);
  SHOULD(AT(place).cdr.car == 6);
  SHOULD(count(UNPACK) == 1);
  SHOULD(count(INTEGER) == 1);
  SHOULD(count(LAMBDA) == 0);
  SHOULD(count(LOCAL) == 0);

  if(fail_p) ERROR("failed expr0");

  char expr1[] = "+ ((> 2) (> 3))";
  run_expr(expr1, place);
  run_down(place);
  place.x = 4; place.y = 3;
  run_down(place);
  place.x = 4; place.y = 4;
  run_down(place);
  SHOULD(AT(place).car.hdr == UNPACK);
  SHOULD(AT(place).cdr.hdr == INTEGER);
  SHOULD(AT(place).cdr.car == 5);
  SHOULD(count(UNPACK) == 1);
  SHOULD(count(INTEGER) == 1);
  SHOULD(count(LAMBDA) == 0);
  SHOULD(count(LOCAL) == 0);

  if(fail_p) ERROR("failed expr1");

  char expr2[] = "* ((+ (1 (2))) ((- (32 (8)))))";
  run_expr(expr2, place);
  run_down(place);
  place.x = 4; place.y = 3;
  run_down(place);
  place.x = 4; place.y = 2;
  run_down(place);
  place.x = 3; place.y = 2;
  run_down(place);
  place.x = 3; place.y = 1;
  run_down(place);
  place.x = 4; place.y = 4;
  run_down(place);
  SHOULD(AT(place).car.hdr == UNPACK);
  SHOULD(AT(place).cdr.hdr == INTEGER);
  SHOULD(AT(place).cdr.car == 72);
  SHOULD(count(UNPACK) == 1);
  SHOULD(count(INTEGER) == 1);
  SHOULD(count(LAMBDA) == 0);
  SHOULD(count(LOCAL) == 0);

  if(fail_p) ERROR("failed expr2");

  /* return indicates success or failure */
  return fail_p;
}
