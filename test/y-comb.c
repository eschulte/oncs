#include "test.h"

int main(int argc, char *argv[]){
  init(argc, argv);
  coord place;
  place.x = 4; place.y = 4;

  char y[] = "#L0 (((#L1 (#S0 ((#S1 (#S1)))))) ((#L1 (#S0 ((#S1 (#S1)))))))";
  string_to_onc(place, y, 0);
  show_all(place);

  /* return indicates success or failure */
  return fail_p;
}
