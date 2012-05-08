#include "test.h"
#define BUF_SIZE 1024

void place_ints(coord place);

int main(int argc, char *argv[]){
  char buf[BUF_SIZE];
  coord place, holder;
  init(argc, argv);
  holder.x = holder.y = place.x = place.y = 4;

  place_ints(place);
  show_all(place);
  duplicate_ptr(AT(place).car, 1, FALSE);
  show_all(place);
  do{
    place = queue[qbeg].place;
    run_queue(); show_all(holder);
    run(place);  show_all(holder);
  } while(queue_population() > 0);

  SHOULD(count(INTEGER) == 8);
  SHOULD(count(LOCAL)   == 7);
  SHOULD(count(LAMBDA)  == 0);
  SHOULD(count(SYMBOL)  == 0);

  place.x = place.y = 4;
  onc_to_string(place, buf, 0);
  debug(1, "(%d,%d):%s\n", place.x, place.y, buf);
  debug(1, "integer:%d local:%d lambda:%d symbol:%d\n",
        count(INTEGER), count(LOCAL), count(LAMBDA), count(SYMBOL));
  
  /* return indicates success or failure */
  return fail_p;
}

void place_ints(coord place){
  char* bits;
  coord tmp;
  /* () */
  AT(place).refs++;
  tmp = open_space(place);
  AT(tmp).refs++;
  LOCAL_SET(place, car, bits, tmp);
  NIL_SET(place, cdr);
  /* (1 _ _ _) */
  place = tmp;
  INTEGER_SET(place, car, 1);
  tmp = open_space(place);
  AT(tmp).refs++;
  LOCAL_SET(place, cdr, bits, tmp);
  /* (_ 2 _ _) */
  place = tmp;
  INTEGER_SET(place, car, 2);
  tmp = open_space(place);
  AT(tmp).refs++;
  LOCAL_SET(place, cdr, bits, tmp);
  /* (_ _ 3 _) */
  place = tmp;
  INTEGER_SET(place, car, 3);
  tmp = open_space(place);
  AT(tmp).refs++;
  LOCAL_SET(place, cdr, bits, tmp);
  /* (_ _ _ 4) */
  place = tmp;
  INTEGER_SET(place, car, 4);
  NIL_SET(place, cdr);
}
