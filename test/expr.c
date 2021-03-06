#include "test.h"
#define BUF_SIZE 1024

int main(int argc, char *argv[]){
  char buf[BUF_SIZE];
  coord place;
  init(argc, argv);
  place.x = place.y = SIZE/2;

  simple_app(place);
  debug(1, "\n");
  fflush(stdout);
  show_world();
  onc_to_string(place, buf, 0);

  debug(1, "buf=%s\n", buf);

  SHOULD(strcmp(buf, "(#L1 (#S1 #S1)) (1 2 3)") == 0);

  /* return indicates success or failure */
  return fail_p;
}
