#include "test.h"

int main(int argc, char *argv[]){
  init(argc, argv);
  coord place, left, right;
  left.x = 4; left.y = 4;

  char y[] = "#L0 (#L1 #S0 (#S1 #S1)) (#L1 #S0 (#S1 #S1))";
  char f[] = "(#L2 (#L3 ((= 0 #S3) 1 (* #S3 (#S2 (- #S3 1))))))";

  /* Y combinator */
  string_to_onc(left, y);

  /* Factorial */
  right = open_space(left);
  string_to_onc(right, f);

  /* list holding Y-comb and factorial */
  place = open_space(left);
  AT(place).refs = 1;
  LOCAL_SET(place, car, left);
  LOCAL_SET(place, cdr, right);

  /* Integer Argument */
  left = place;
  place = open_space(left);
  AT(place).refs = 1;
  LOCAL_SET(place, car, left);
  INTEGER_SET(place, cdr, 4);
  show_all(place);

  /* The resulting structure embedded in the space:
   *
   * (((#L0 (#L1 #S0 (#S1 #S1))
   *        (#L1 #S0 (#S1 #S1)))
   *   (#L2 (#L3 ((= #S3 0) 1 (* #S3 (#S2 (- #S3 1)))))))
   *  4)
   */

  fix(place);

  SHOULD(count(INTEGER) == 1);
  SHOULD(count(LAMBDA) == 0);
  SHOULD(count(SYMBOL) == 0);
  SHOULD(AT(place).cdr.hdr == INTEGER);
  SHOULD(AT(place).cdr.car == 24);

  /* return indicates success or failure */
  return fail_p;
}
