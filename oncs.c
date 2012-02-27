#include <stdio.h>
#define SIZE 100
/*
  ptr.hdr meaning:
  0 -> nil
  1 -> local pointer (car,cdr)
  2 -> integer add car and cdr
  3 -> when symbol interpret car as the symbol
*/
typedef struct { int hdr, car, cdr; } ptr;
typedef struct { ptr car, cdr; } onc;

onc world[SIZE][SIZE];

int main(int argc, char* argv){
  printf("oncs are not conses: ");
  world[0][0].car.hdr = 3;
  world[0][0].car.car = 0;
  world[0][0].cdr.hdr = 0;
  printf(" world[0][0].hdr=%d\n", world[0][0].car.hdr);
}
