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
    /* cycle around original place */
    switch(tmp % 4){
    case 1: free.y = (free.y + tmp/2) % SIZE; break;
    case 2: free.x = (free.x + tmp/2) % SIZE; break;
    case 3: free.y = (free.y - tmp/2) % SIZE; break;
    case 0: free.x = (free.x - tmp/2) % SIZE; break;
    }
    if(tmp >= SIZE * SIZE)
      printf("ERROR: exhausted free space\n");
  } while(AT(free).refs != 0);
  return free;
}

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
