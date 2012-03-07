#include <stdio.h>
#include "oncs.h"

onc world[SIZE][SIZE];

/* return the coord of the nearest open space */
coord open_space(coord place){
  coord free;
  free.x = place.x;
  free.y = place.y;
  int tmp = 1;
  do{
    /* cycle around original place
     * ===========================
     *        ^       1
     *        |    ^ --> |
     *       3|   1|     |2
     *        | <------- v
     *               2
     */
    switch(tmp % 4){
    case 1: free.y = (free.y + tmp/2) % SIZE; break;
    case 2: free.x = (free.x + tmp/2) % SIZE; break;
    case 3: free.y = (free.y - tmp/2) % SIZE; break;
    case 0: free.x = (free.x - tmp/2) % SIZE; break;
    }
    if(tmp >= SIZE * SIZE)
      printf("ERROR: exhausted free space\n");
  } while(AT(free).refs != 0);
  AT(free).refs = 1;
  return free;
}

coord duplicate(coord from, coord to){
  switch(AT(from).car.hdr){
  case NIL: break;
  case LOCAL:                   /* copy reference */
    coord from_car;
    from_car.x = AT(from.car.car);
    from_car.y = AT(from.car.cdr);
    coord to_car;
    to_car = open_space(to);
    AT(to).car.hdr = LOCAL;
    AT(to).car.car = to_car.x;
    AT(to).car.cdr = to_car.y;
    duplicate(from_car, to_car);
    break;
  case INTEGER:
  case SYMBOL:
  case LAMBDA:                  /* copy literal */
    AT(to).car = AT(from).car;  /* TODO: need to explicitly duplicate? */
    break;
  }
  if(AT(from).car != NIL){
    tmp = open_space(to)
  }
};

/* TODO: this will inf loop */
int main(int argc, char* argv){
  printf("oncs are not conses.\n");
  coord place;
  place.x = 0;
  place.y = 0;
  printf("world[0][0].refs=%d\n", AT(place).refs);
  quit: /* exit successfully */   return 0;
  bail: /* exit with an errors */ return 1;
}
