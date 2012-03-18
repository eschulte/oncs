#include "test.h"

int main(int argc, char *argv[]){
  init(argc, argv);
  int i, j;
  coord place, new;

  /* setup world */
  new.x = 1;
  new.y = place.x = place.y = 0;
  LAMBDA_SET(place, 32);
  LOCAL_SET(place, cdr, new);
  SYMBOL_SET(new, car, 32);

  SHOULD(population() == 2);
  show_world();

  /* duplicate 16 times */
  int flip,counter;
  flip = counter = 0;
  do{
    if(flip) new.x = WRAP(new.x+1);
    else     new.y = WRAP(new.y+1);
    flip = !flip;
    if(AT(new).refs == 0){
      counter++;
      duplicate(new, place);
      show_world();
      SHOULD(population() == 2 + (2 * counter));
    }
  } while (counter <= 16);

  /* return indicates success or failure */
  return fail_p;
}
