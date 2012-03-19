#include "test.h"
#define BUF_SIZE 1024

int main(int argc, char *argv[]){
  init(argc, argv);

  debug("simple parsing of an integer\n");
  char buf1[] = "one 3241z ";
  SHOULD(read_int(buf1, 4) == 3241);
  SHOULD(read_int(buf1, 5) == 241);
  SHOULD(read_int(buf1, 6) == 41);
  SHOULD(read_int(buf1, 7) == 1);

  char buf[] = "((#L#S1 #S1 #S1 ) 1 2 3 )";
  coord place;
  place.x = place.y = 3;
  string_to_onc(place, buf, 0);
  show_world();

  /* return indicates success or failure */
  return fail_p;
}
