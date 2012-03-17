#include <stdlib.h>
#include <stdio.h>
#include "oncs.h"

onc world[SIZE][SIZE];
msg queue[QLENGTH];
int qbeg, qend = 0;

void enqueue(coord coord, ptr mcar, ptr mcdr){
  int i;
  for(i=0;i<QLENGTH;i++)
    if(queue[QWRAP(qbeg + i)].mcar.hdr == NIL)
      break;
  if(i == (QLENGTH - 1)){
    printf("ERROR: queue overflow\n");
    exit(1);
  }
  queue[QWRAP(qbeg + i)].coord = coord;
  queue[QWRAP(qbeg + i)].mcar = mcar;
  queue[QWRAP(qbeg + i)].mcdr = mcdr;
  qbeg = QWRAP(qbeg+1);
}

msg dequeue(){
  int i;
  for(i=0;i<QLENGTH;i++){
    if(QWRAP(qend+i) == qbeg)
      break;
    if(queue[QWRAP(qend+i)].mcar.hdr != NIL){
      qend = QWRAP(qend+i+1);
      return queue[QWRAP(qend-1)];
    }
  }
}

void show_ptr(ptr ptr){
  switch(ptr.hdr){
  case NIL:     printf("_"); break;
  case LOCAL:   printf("^"); break;
  case INTEGER: printf("i"); break;
  case SYMBOL:  printf("s"); break;
  case LAMBDA:  printf("l"); break;
  default:      printf("?"); break;
  }
}

void show_world(){
  onc tmp;
  int i, j;
  printf("\n");
  fflush(stdout);
  for(i=0;i<SIZE;i++) {
    for(j=0;j<SIZE;j++) {
      tmp = world[j][i];
      if(tmp.refs > 0) {
        printf("["); show_ptr(tmp.mcar);
        printf(","); show_ptr(tmp.car);
        printf(","); show_ptr(tmp.cdr);
        printf("]");
      } else printf("       ");
      if(j<(SIZE-1)) printf(" ");
    }
    printf("\n");
    fflush(stdout);
  }
}

/* return the coord of the nearest open space */
coord open_space(coord place){
  /* cycle around original place
   * ===========================
   *        ^       1
   *        |    ^ --> |
   *       3|   1|     |2
   *        | <------- v
   *               2
   */
  int index, cntrl, tried;
  index = cntrl = tried = 0;
  do {
    if (index == (cntrl/2)) {
      index = 0;
      cntrl++;
    } else {
      index++;
      tried++;
      switch(cntrl % 4){
      case 0: place.y = WRAP(place.y + 1); break;
      case 1: place.x = WRAP(place.x + 1); break;
      case 2: place.y = WRAP(place.y + (SIZE - 1)); break;
      case 3: place.x = WRAP(place.x + (SIZE - 1)); break;
      }
    }
    if (AT(place).refs == 0) break;
  } while (tried <= SIZE*SIZE);
  if (tried >= SIZE*SIZE) {
    printf("ERROR: exhausted world\n");
    exit(1);
  } else {
    return place;
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
  case LAMBDA: to = from; break;
  }
}

void duplicate(coord from, coord to){
  duplicate_ptr(AT(from).car, AT(to).car, from, to);
  duplicate_ptr(AT(from).cdr, AT(to).cdr, from, to);
  duplicate_ptr(AT(from).mcar, AT(to).mcar, from, to);
  duplicate_ptr(AT(from).mcdr, AT(to).mcdr, from, to);
}

void replace_ptr(int var, ptr to, coord to_coord, ptr new, coord new_coord){
  switch(to.hdr){
  case NIL:
  case INTEGER: break;
  case SYMBOL: if(to.car != var) break;
  case LAMBDA: if(to.hdr == var) break;
  case LOCAL: duplicate_ptr(new, to, new_coord, to_coord); break;
  }
}

void run(coord place){
  coord tmp_coord;
  switch(AT(place).mcar.hdr){
  case INTEGER:                 /* update number of references */
    AT(place).refs += AT(place).mcar.car;
    if(AT(place).car.hdr == LOCAL){
      tmp_coord.x = AT(place).car.car;
      tmp_coord.y = AT(place).car.cdr;
      enqueue(tmp_coord, AT(place).mcar, AT(place).mcdr);
    }
    if(AT(place).cdr.hdr == LOCAL){
      tmp_coord.x = AT(place).cdr.car;
      tmp_coord.y = AT(place).cdr.cdr;
      enqueue(tmp_coord, AT(place).mcar, AT(place).mcdr);
    }
    break;
  default: /* LAMBDA application */ break;
  }
  AT(place).mcar.hdr = NIL;
}
