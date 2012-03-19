#include "test.h"
#define BUF_SIZE 1024

int main(int argc, char *argv[]){
  init(argc, argv);
  coord place;
  int index;

  debug("simple parsing of an integer\n");
  char buf1[] = "one 3241z ";
  index = 4;
  SHOULD(read_int(buf1, &index) == 3241);
  index = 5;
  SHOULD(read_int(buf1, &index) == 241);
  index = 6;
  SHOULD(read_int(buf1, &index) == 41);
  index = 7;
  SHOULD(read_int(buf1, &index) == 1);
  SHOULD(index == 8);

  char buf2[] = "#S1";
  place.x = place.y = 3;
  string_to_onc(place, buf2, 0);
  show_world();
  clear_world();

  char buf3[] = "#S1 678";
  place.x = place.y = 3;
  string_to_onc(place, buf3, 0);
  show_world();
  clear_world();

  char buf5[] = "((#L#S1 #S1 #S1 ) 1 2 3 )";

  /* return indicates success or failure */
  return fail_p;
}
