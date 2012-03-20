#include "test.h"
#define BUF_SIZE 1024

void insert_into_world(coord place, char *buf){
  clear_world();
  place.x = place.y = 3;
  debug(1, "exp:%s\n", buf);
  string_to_onc(place, buf, 0);
  show_world();
}

int main(int argc, char *argv[]){
  init(argc, argv);
  coord place;
  int index;

  debug(1, "simple parsing of an integer\n");
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
  insert_into_world(place, buf2);
  SHOULD(count(SYMBOL) == 1);
  SHOULD(population() == 1);

  char buf3[] = "#S1 678";
  insert_into_world(place, buf3);
  SHOULD(count(SYMBOL) == 1);
  SHOULD(count(INTEGER) == 1);
  SHOULD(population() == 1);

  char buf4[] = "#S1 (#S2 678)";
  insert_into_world(place, buf4);
  SHOULD(count(SYMBOL) == 2);
  SHOULD(count(INTEGER) == 1);
  SHOULD(population() == 2);

  char buf5[] = "(1 2)";
  insert_into_world(place, buf5);
  SHOULD(count(INTEGER) == 2);
  SHOULD(population() == 2);

  char buf6[] = "(#L1 (#S1 #S1)) (1 (2 3))";
  insert_into_world(place, buf6);
  SHOULD(count(LAMBDA) == 1);
  SHOULD(count(SYMBOL) == 2);
  SHOULD(count(INTEGER) == 3);

  char buf7[] = "((#L1 (#S1 #S1)) (1 (2 3)))";
  insert_into_world(place, buf7);
  SHOULD(count(LAMBDA) == 1);
  SHOULD(count(SYMBOL) == 2);
  SHOULD(count(INTEGER) == 3);

  char buf8[] = "(#L1 (#L2 #S1)) ((#L1 (#L2 #S1)) (3 4))";
  insert_into_world(place, buf8);
  SHOULD(count(LAMBDA) == 4);

  char buf9[] = "#L1 (#L2 #S1)";
  insert_into_world(place, buf9);
  SHOULD(count(LAMBDA) == 2);
  SHOULD(count(SYMBOL) == 1);

  /* return indicates success or failure */
  return fail_p;
}
