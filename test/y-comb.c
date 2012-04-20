#include "test.h"

int main(int argc, char *argv[]){
  init(argc, argv);
  coord place;
  place.x = 4; place.y = 4;

  /* (λ m (λ f (m (λ a ((f f) a))))
   *      (λ f (m (λ a ((f f) a)))))
   *
   * m = #L0
   * f = #L1
   * a = #L2
   */
  char w[] =
    "(#L0 (#L1 (#S0 #L2 ((#S1 #S1) #S2)))"
    "     (#L1 (#S0 #L2 ((#S1 #S1) #S2))))";

  /* (λ r (λ n ((= 0 n) 1 (* n (r (- 1 n)))))
   *
   * r = #L3
   * n = #L4
   */
  char f[] =
    "(#L3 (#L4 (= 0 #S4) 1 (* #S4 (#S3 (- 1 #S4)))))";

  /* (((λ m (λ f (m (λ a ((f f) a))))
   *        (λ f (m (λ a ((f f) a)))))
   *   (λ r (λ n ((= 0 n) 1 (* n (r (- 1 n)))))))
   *  2)
   *
   * m = #L0
   * f = #L1
   * a = #L2
   * r = #L3
   * n = #L4
   */

  char expr[] =
    "(((#L0 (#L1 (#S0 #L2 ((#S1 #S1) #S2)))"
    "       (#L1 (#S0 #L2 ((#S1 #S1) #S2))))"
    "  (#L3 (#L4 ((= 0 #S4) 1 (* #S4 (#S3 (- 1 #S4)))))))"
    " 2)";
  string_to_onc(place, expr);
  show_all(place);
  fix(place);

  SHOULD(count(INTEGER) == 2);
  SHOULD(count(LAMBDA) == 0);
  SHOULD(count(SYMBOL) == 0);
  SHOULD(AT(place).cdr.hdr == INTEGER);
  SHOULD(AT(place).cdr.car == 24);

  /* return indicates success or failure */
  return fail_p;
}
