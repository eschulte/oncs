#include "test.h"
#define BUF_SIZE 1024

int main(int argc, char *argv[]){
  coord place, target;
  init(argc, argv);
  place.x = place.y = 4;
  target.x = target.y = 6;

  AT(place).refs = 1;
  AT(place).car.hdr = LOCAL;
  PTR_OF_COORD(AT(place).car, target);
  AT(place).car.hdr = NIL;
  AT(target).refs = 1;
  AT(target).car.hdr = INTEGER;
  AT(target).car.car = 2;
  AT(target).cdr.hdr = INTEGER;
  AT(target).cdr.car = 2;

  update_ref_msg(place, 1);
  run_down(place);
  
  place.x = place.y = 4;
  SHOULD(AT(place).refs == 2);
  target.x = target.y = 6;
  SHOULD(AT(target).refs == 2);

  place.x = place.y = 4;
  update_ref_msg(place, -1);
  run_down(place);

  place.x = place.y = 4;
  show_all(place);
  SHOULD(AT(place).refs == 1);
  target.x = target.y = 6;
  SHOULD(AT(target).refs == 1);

  place.x = place.y = 4;
  update_ref_msg(place, -1);
  run_down(place);

  place.x = place.y = 4;
  show_all(place);
  SHOULD(AT(place).refs == 0);
  target.x = target.y = 6;
  SHOULD(AT(target).refs == 0);
  
  /* return indicates success or failure */
  return fail_p;
}
