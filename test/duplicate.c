#include "test.h"
#define BUF_SIZE 1024

void place_ints(coord place);

int main(int argc, char *argv[]){
  char buf[BUF_SIZE];
  coord place, holder;
  msg msg;
  init(argc, argv);
  place.x = place.y = 4;
  holder.x = holder.y = 6;
  AT(holder).refs = 1;
  AT(holder).car.hdr = INTEGER;
  AT(holder).car.car = 8;
  AT(holder).cdr.hdr = NIL;

  place_ints(place);
  msg.place = place;
  msg.mcar.hdr = DUPLICATE;
  PTR_OF_COORD(msg.mcar, holder);
  msg.mcdr.car = 0;
  msg.mcdr.cdr = 0;
  enqueue(msg);
  fix(holder);

  SHOULD(count(INTEGER) == 8);
  SHOULD(count(LOCAL)   == 8);
  SHOULD(count(LAMBDA)  == 0);
  SHOULD(count(SYMBOL)  == 0);

  place.x = place.y = 4;
  onc_to_string(place, buf, 0);
  debug(1, "(%d,%d):%s\n", place.x, place.y, buf);
  debug(1, "integer:%d local:%d lambda:%d symbol:%d\n",
        count(INTEGER), count(LOCAL),
        count(LAMBDA), count(SYMBOL));

  /* return indicates success or failure */
  return fail_p;
}

void place_ints(coord place){
  coord tmp;
  /* () */
  AT(place).refs++;
  tmp = open_space(place);
  AT(tmp).refs++;
  LOCAL_SET(place, car, tmp);
  NIL_SET(place, cdr);
  /* (1 _ _ _) */
  place = tmp;
  INTEGER_SET(place, car, 1);
  tmp = open_space(place);
  AT(tmp).refs++;
  LOCAL_SET(place, cdr, tmp);
  /* (_ 2 _ _) */
  place = tmp;
  INTEGER_SET(place, car, 2);
  tmp = open_space(place);
  AT(tmp).refs++;
  LOCAL_SET(place, cdr, tmp);
  /* (_ _ 3 _) */
  place = tmp;
  INTEGER_SET(place, car, 3);
  tmp = open_space(place);
  AT(tmp).refs++;
  LOCAL_SET(place, cdr, tmp);
  /* (_ _ _ 4) */
  place = tmp;
  INTEGER_SET(place, car, 4);
  NIL_SET(place, cdr);
}
