#include "test.h"

int main(int argc, char *argv[]){
  init(argc, argv);
  coord place, left, right;
  left.x = 4; left.y = 4;

  char f[] = "(#L2 (#L3 ((= #S3 0) 1 (* #S3 (#S2 (- #S3 1))))))";

  /* Factorial */
  string_to_onc(left, f);

  /* list holding factorial and argument */
  place = open_space(left);
  AT(place).refs = 1;
  LOCAL_SET(place, car, left);
  INTEGER_SET(place, cdr, 0);

  show_all(place);
  fix(place);

  place.x = 3; place.y = 6;
  run_down(place);
  place.x = 3; place.y = 1;
  run_down(place);
  place.x = 4; place.y = 8;
  run_down(place);

  fix(place);

  SHOULD(count(INTEGER) == 1);
  SHOULD(count(LAMBDA) == 0);
  SHOULD(count(SYMBOL) == 0);
  SHOULD(AT(place).cdr.hdr == INTEGER);
  SHOULD(AT(place).cdr.car == 1);

  /* return indicates success or failure */
  return fail_p;
}
