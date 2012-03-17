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

  /* duplicate four more times */
  int k,l,counter;
  counter = 0;
  for(l=1;l<=4;l++){
    for(k=1;k<=4;k++){
      counter++;
      new.x = 4+k % SIZE;
      new.y = 4+l % SIZE;
      duplicate(new, place);
      if(verbose_p) show_world();
      SHOULD(population() == (4 + (2 * counter)));
    }
  }

  /* return indicates success or failure */
  return fail_p;
}
