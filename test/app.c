#include "test.h"

int main(int argc, char *argv[]){
  init(argc, argv);
  int i, j;
  coord place;

  place.x = place.y = 4;
  simple_app(place);

  run(place);  show_world();
  run_queue(); show_world();

  place.x = 4; place.y = 3;
  run(place);  show_world();
  run_queue(); show_world();
  
  place.x = 4; place.y = 2;
  run(place);  show_world();
  run_queue(); show_world();
  
  place.x = 4; place.y = 1;
  run(place);  show_world();
  run_queue(); show_world();
  
  /* return indicates success or failure */
  return fail_p;
}
