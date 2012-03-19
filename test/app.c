#include "test.h"
#define BUF_SIZE 1024

int main(int argc, char *argv[]){
  init(argc, argv);
  int i, j;
  char buf[BUF_SIZE];
  coord place;
  place.x = place.y = 4;

  simple_app(place); show_world();
  run(place);        show_world();
  do{
    place = queue[qbeg].coord;
    run_queue(); show_world();
    run(place);  show_world();
  } while(queue_population() > 0);

  SHOULD(count(INTEGER) == 6);
  SHOULD(count(LOCAL) == 7);
  SHOULD(count(LAMBDA) == 0);
  SHOULD(count(SYMBOL) == 0);

  place.x = place.y = 4;
  onc_to_string(place, buf, 0);
  debug("(%d,%d):%s\n", place.x, place.y, buf);
  
  /* return indicates success or failure */
  return fail_p;
}
