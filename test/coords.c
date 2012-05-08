#include "test.h"

int main(int argc, char *argv[]){
  coord coord;
  ptr ptr;
  init(argc, argv);
  debug(2, "starting coord test\n");

  coord.X = 1;
  coord.Y = 2;
  coord.x = 3;
  coord.y = 4;
  
  PTR_OF_COORD(ptr, coord);

  SHOULD(ptr.car == 513);
  debug(1, "ptr.car %d == 513\n", ptr.car);
  SHOULD(ptr.cdr == 1027);
  debug(1, "ptr.cdr %d == 1027\n", ptr.cdr);
  
  coord.X = coord.Y = coord.x = coord.y = 0;

  COORD_OF_PTR(coord, ptr);
  
  SHOULD(coord.X == 1);
  debug(1, "coord.X %d == 1\n", coord.X);
  SHOULD(coord.Y == 2);
  debug(1, "coord.Y %d == 2\n", coord.Y);
  SHOULD(coord.x == 3);
  debug(1, "coord.x %d == 3\n", coord.x);
  SHOULD(coord.y == 4);
  debug(1, "coord.y %d == 4\n", coord.y);

  /* return indicates success or failure */
  return fail_p;
}
