#include "test.h"
#define BUF_SIZE 1024

void app_2(coord place);

int main(int argc, char *argv[]){
  init(argc, argv);
  int i, j;
  char buf[BUF_SIZE];
  coord place, holder;
  holder.x = holder.y = place.x = place.y = 4;

  app_2(place); show_all(holder);
  run(place);        show_all(holder);
  do{
    place = queue[qbeg].coord;
    run_queue(); show_all(holder);
    run(place);  show_all(holder);
  } while(queue_population() > 0);

  SHOULD(count(INTEGER) == 1);
  SHOULD(count(LOCAL)   == 2);
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

void app_2(coord place){
  coord tmp1, tmp2;
  /* setup world: ((lambda x (x)) (1)) */
  AT(place).refs++;
  debug(2, "(%d,%d) -- ((lambda x (x)) (1))\n", place.x, place.y);
  tmp1 = open_space(place);
  AT(tmp1).refs++;
  LOCAL_SET(place, car, tmp1);
  debug(2, "(%d,%d) -- (lambda x (x))\n", tmp1.x, tmp1.y);
  tmp2 = open_space(place);
  AT(tmp2).refs++;
  LOCAL_SET(place, cdr, tmp2);
  debug(2, "(%d,%d) -- (1)\n", tmp2.x, tmp2.y);
  /* (lambda x (x)) */
  place = tmp1;
  LAMBDA_SET(place, 1);
  tmp1 = open_space(place);
  AT(tmp1).refs++;
  debug(2, "(%d,%d) -- (x)\n", tmp1.x, tmp1.y);
  LOCAL_SET(place, cdr, tmp1);
  /* (x) */
  place = tmp1;
  SYMBOL_SET(place, car, 1);
  NIL_SET(place, cdr);
  /* ((1)) */
  place = tmp2;
  tmp2 = open_space(place);
  AT(tmp2).refs++;
  LOCAL_SET(place, car, tmp2);
  INTEGER_SET(tmp2, car, 1);
  NIL_SET(tmp2, cdr);
}
