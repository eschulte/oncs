#include "test.h"

int main(int argc, char *argv[]){
  init(argc, argv);
  int i, j;
  coord place;

  place.x = place.y = 4;
  simple_app(place);

  if(verbose_p) show_world();

  /* return indicates success or failure */
  return fail_p;
}
