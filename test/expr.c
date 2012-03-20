#include "test.h"
#define BUF_SIZE 1024

int main(int argc, char *argv[]){
  init(argc, argv);

  char buf[BUF_SIZE];
  coord place;
  place.x = place.y = 4;

  simple_app(place); show_world();
  onc_to_string(place, buf, 0);

  debug(1, "buf=%s\n", buf);

  SHOULD(strcmp(buf, "((#L1 #S1 #S1 ) 1 2 3 )") == 0);

  /* return indicates success or failure */
  return fail_p;
}
