#include "test.h"

int main(int argc, char *argv[]){
  init(argc, argv);
  int i, j;
  coord place;
  place.x = place.y = 4;

  simple_app(place); show_world();
  run(place);        show_world();
  do{
    place = queue[qbeg].coord;
    run_queue(); show_world();
    run(place);  show_world();
  } while(queue_population() > 0);

  debug("INTEGER %d=%d\n", count(INTEGER), 6);
  SHOULD(count(INTEGER) == 6);
  debug("LOCAL %d=%d\n", count(LOCAL), 7);
  SHOULD(count(LOCAL) == 7);
  debug("LAMBDA %d=%d\n", count(LAMBDA), 0);
  SHOULD(count(LAMBDA) == 0);
  debug("SYMBOL %d=%d\n", count(SYMBOL), 0);
  SHOULD(count(SYMBOL) == 0);

  /* return indicates success or failure */
  return fail_p;
}
