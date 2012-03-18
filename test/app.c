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

  /* return indicates success or failure */
  return fail_p;
}
