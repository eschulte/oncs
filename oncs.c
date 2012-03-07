#include <stdlib.h>
#include <stdio.h>
#include "oncs.h"

onc world[SIZE][SIZE];

/* return the coord of the nearest open space */
coord open_space(coord place){
  coord free;
  free.x = place.x;
  free.y = place.y;
  int tmp = 1;
  /* cycle around original place
   * ===========================
   *        ^       1
   *        |    ^ --> |
   *       3|   1|     |2
   *        | <------- v
   *               2
   */
  for(tmp=1; tmp<SIZE*SIZE; tmp++){
    switch(tmp % 4){
    case 1: free.y = WRAP(free.y + tmp/2); break;
    case 2: free.x = WRAP(free.x + tmp/2); break;
    case 3: free.y = WRAP(free.y - tmp/2); break;
    case 0: free.x = WRAP(free.x - tmp/2); break;
    }
    if(AT(free).refs == 0) break;
  }
  if(tmp == SIZE*SIZE) {
    printf("ERROR: exhausted free space\n");
    exit(1);
  } else {
    AT(free).refs = 1;
    return free;
  }
}

void duplicate_ptr(ptr from, ptr to, coord from_coord, coord to_coord){
  coord from_ptr, to_ptr;
  switch(from.hdr){
  case NIL: to.hdr = NIL; break;
  case LOCAL:
    from_ptr.x = WRAP(from_coord.x + from.car);
    from_ptr.y = WRAP(from_coord.y + from.cdr);
    to_ptr = open_space(to_coord);
    to.hdr = LOCAL;
    to.car = to_ptr.x;
    to.cdr = to_ptr.y;
    duplicate(from_ptr, to_ptr);
    break;
  case INTEGER:
  case SYMBOL:
  case LAMBDA:
    COPY_PTR(to,from);
    break;
  }
}

void duplicate(coord from, coord to){
  duplicate_ptr(AT(from).car, AT(to).car, from, to);
  duplicate_ptr(AT(from).cdr, AT(to).cdr, from, to);
  duplicate_ptr(AT(from).msg, AT(to).msg, from, to);
}

void replace_ptr(int var, ptr to, coord to_coord, ptr new, coord new_coord){
  switch(to.hdr){
  case NIL:
  case INTEGER: break;
  case LAMBDA: if(to.car == var) break;
  case SYMBOL: if(to.car != var) break;
  case LOCAL: duplicate_ptr(new, to, new_coord, to_coord); break;
  }
}

/* TODO: this will inf loop */
int main(int argc, char* argv){
  printf("oncs are not conses.\n");
  coord place;
  place.x = 0;
  place.y = 0;
  printf("world[0][0].refs=%d\n", AT(place).refs);
  return 0;
}
