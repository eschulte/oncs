#include "test.h"
#define BUF_SIZE 1024

int main(int argc, char *argv[]){
  coord place, target, temp;
  init(argc, argv);
  place.x = place.y = 0;
  target.x = target.y = SIZE/2;

  AT(place).refs = 1;
  AT(place).car.hdr = LOCAL;
  PTR_OF_COORD(AT(place).car, target);
  AT(place).cdr.hdr = NIL;
  AT(target).refs = 1;
  AT(target).car.hdr = INTEGER;
  AT(target).car.car = 2;
  AT(target).cdr.hdr = INTEGER;
  AT(target).cdr.car = 2;

  COORD_OF_PTR(temp, AT(place).car);
  debug(1, "AT(place).car -> (%d,%d,%d,%d,%d)\n",
        AT(place).car.hdr, temp.x, temp.y, temp.X, temp.Y);
  show_all(place);

  update_ref_msg(place, 1);
  run_down(place);
  
  debug(1, "AT(place).refs %d == 2\n", AT(place).refs);
  SHOULD(AT(place).refs == 2);
  debug(1, "AT(target).refs %d == 2\n", AT(target).refs);
  SHOULD(AT(target).refs == 2);

  update_ref_msg(place, -1);
  run_down(place);

  show_all(place);
  debug(1, "AT(place).refs %d == 1\n", AT(place).refs);
  SHOULD(AT(place).refs == 1);
  debug(1, "AT(target).refs %d == 1\n", AT(target).refs);
  SHOULD(AT(target).refs == 1);

  update_ref_msg(place, -1);
  run_down(place);

  show_all(place);
  debug(1, "AT(place).refs %d == 0\n", AT(place).refs);
  SHOULD(AT(place).refs == 0);
  debug(1, "AT(target).refs %d == 0\n", AT(target).refs);
  SHOULD(AT(target).refs == 0);
  
  /* return indicates success or failure */
  return fail_p;
}
