#include "test.h"

int main(int argc, char *argv[]){
  init(argc, argv);
  int i, j;
  coord place, new1, new2;

  /* setup world: ((lambda x (x x)) (1 2 3)) */
  place.x = place.y = 4;
  new1 = open_space(place);
  LOCAL_SET(place, car, new1);
  new2 = open_space(place);
  LOCAL_SET(place, cdr, new2);
  /* (lambda x (x x)) */
  place = new1;
  LAMBDA_SET(place, 1);
  new1 = open_space(place);
  LOCAL_SET(place, cdr, new1);
  /* (x x) */
  place = new1;
  SYMBOL_SET(place, car, 1);
  new1 = open_space(place);
  LOCAL_SET(place, cdr, new1);
  SYMBOL_SET(new1, car, 1);
  NIL_SET(new1, cdr);
  /* (1 2 3) */
  place = new2;
  INTEGER_SET(place, car, 1);
  new2 = open_space(place);
  LOCAL_SET(place, cdr, new2);
  place = new2;
  INTEGER_SET(place, car, 2);
  new2 = open_space(place);
  place = new2;
  INTEGER_SET(place, car, 3);
  NIL_SET(place, cdr);

  if(verbose_p) show_world();

  /* return indicates success or failure */
  return fail_p;
}
