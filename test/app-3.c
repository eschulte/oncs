#include "test.h"
#define BUF_SIZE 1024

int main(int argc, char *argv[]){
  init(argc, argv);
  int i, j;
  char buf[BUF_SIZE];
  coord place, holder;
  holder.x = holder.y = place.x = place.y = 4;

  simple_app(place); show_all(holder);
  run(place);        show_all(holder);
  do{
    place = queue[qbeg].coord;
    run_queue(); show_all(holder);
    run(place);  show_all(holder);
  } while(queue_population() > 0);

  SHOULD(count(INTEGER) == 6);
  SHOULD(count(LOCAL)   == 8);
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
