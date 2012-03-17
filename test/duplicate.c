#include "test.h"

int main(int argc, char *argv[]){
  init(argc, argv);
  int i, j;
  coord place, new;

  /* setup world */
  new.x = 3;
  new.y = place.x = place.y = 2;
  place_lambda(place, 32);
  place_cdr_local(place, new);
  place_symbol(new, 32);

  SHOULD(population() == 2);
  if(verbose_p) show_world();

  /* duplicate two linked oncs */
  new.x = new.y = 4;
  duplicate(new, place);

  SHOULD(population() == 4);
  if(verbose_p) show_world();

  /* return indicates success or failure */
  return fail_p;
}
