#include "test.h"
#define BUF_SIZE 1024

void insert_into_world(coord place, char *buf){
  clear_world();
  place.x = place.y = 3;
  debug(1, "exp:%s\n", buf);
  string_to_onc(place, FALSE, buf);
  show_all(place);
}

int main(int argc, char *argv[]){
  coord place;
  unsigned int index;
  char buf1[] = "one 3241z ";
  char buf2[] = "(#S1)";
  char buf3[] = "#S1 . 678";
  char buf4[] = "#S1 (#S2 678)";
  char buf5[] = "(1 2)";
  char buf6[] = "(#L1 (#S1 #S1)) (1 (2 3))";
  char buf7[] = "(#L1 (#S1 #S1)) (1 2 3)";
  char buf8[] = "(#L1 (#L2 #S1)) ((#L1 (#L2 #S1)) (3 4))";
  char buf9[] = "#L1 (#L2 #S1)";
  init(argc, argv);
  place.x = place.y = 4;

  debug(1, "simple parsing of an integer\n");
  index = 4;
  SHOULD(read_int(buf1, &index) == 3241);
  index = 5;
  SHOULD(read_int(buf1, &index) == 241);
  index = 6;
  SHOULD(read_int(buf1, &index) == 41);
  index = 7;
  SHOULD(read_int(buf1, &index) == 1);
  SHOULD(index == 8);
  if(fail_p) ERROR("failed buf1");

  insert_into_world(place, buf2);
  SHOULD(count(LOCAL)  == 1);
  SHOULD(count(SYMBOL) == 1);
  SHOULD(population()  == 2);
  if(fail_p) ERROR("failed buf2");

  insert_into_world(place, buf3);
  SHOULD(count(SYMBOL)  == 1);
  SHOULD(count(INTEGER) == 1);
  SHOULD(population()   == 1);
  if(fail_p) ERROR("failed buf3");

  insert_into_world(place, buf4);
  SHOULD(count(SYMBOL) == 2);
  SHOULD(count(INTEGER) == 1);
  SHOULD(population() == 4);
  if(fail_p) ERROR("failed buf4");

  insert_into_world(place, buf5);
  SHOULD(count(INTEGER) == 2);
  SHOULD(population() == 3);
  if(fail_p) ERROR("failed buf5");

  insert_into_world(place, buf6);
  SHOULD(count(LAMBDA) == 1);
  SHOULD(count(SYMBOL) == 2);
  SHOULD(count(INTEGER) == 3);
  if(fail_p) ERROR("failed buf6");

  insert_into_world(place, buf7);
  SHOULD(count(LAMBDA) == 1);
  SHOULD(count(SYMBOL) == 2);
  SHOULD(count(INTEGER) == 3);
  if(fail_p) ERROR("failed buf7");

  insert_into_world(place, buf8);
  SHOULD(count(LAMBDA) == 4);
  if(fail_p) ERROR("failed buf8");

  insert_into_world(place, buf9);
  SHOULD(count(LAMBDA) == 2);
  SHOULD(count(SYMBOL) == 1);
  if(fail_p) ERROR("failed buf9");

  /* return indicates success or failure */
  return fail_p;
}
