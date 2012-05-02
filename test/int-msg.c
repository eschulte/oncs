#include "test.h"
#define BUF_SIZE 1024

int main(int argc, char *argv[]){
  coord place;
  init(argc, argv);
  place.x = place.y = 4;

  AT(place).refs = 1;
  AT(place).car.hdr = LOCAL;
  AT(place).car.car = AT(place).car.cdr = 6;
  place.x = place.y = 6;
  AT(place).refs = 1;
  AT(place).car.hdr = INTEGER;
  AT(place).car.car = 2;
  AT(place).cdr.hdr = INTEGER;
  AT(place).cdr.car = 2;

  place.x = place.y = 4;
  update_ref_msg(place, 1);
  run_down(place);
  
  place.x = place.y = 4;
  SHOULD(AT(place).refs == 2);
  place.x = place.y = 6;
  SHOULD(AT(place).refs == 2);

  place.x = place.y = 4;
  update_ref_msg(place, -1);
  run_down(place);

  place.x = place.y = 4;
  show_all(place);
  SHOULD(AT(place).refs == 1);
  place.x = place.y = 6;
  SHOULD(AT(place).refs == 1);

  place.x = place.y = 4;
  update_ref_msg(place, -1);
  run_down(place);

  place.x = place.y = 4;
  show_all(place);
  SHOULD(AT(place).refs == 0);
  place.x = place.y = 6;
  SHOULD(AT(place).refs == 0);
  
  /* return indicates success or failure */
  return fail_p;
}
