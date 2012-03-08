#include <stdlib.h>
#include <stdio.h>
#include "oncs.h"

onc world[SIZE][SIZE];
msg queue[QLENGTH]; /* TODO: store queued messages in the onc world */
int qbeg, qend = 0;

void enqueue(coord coord, ptr msg){
  int i;
  for(i=0;i<QLENGTH;i++)
    if(queue[QWRAP(qbeg + i)].msg.hdr == NIL)
      break;
  if(i == (QLENGTH - 1)){
    printf("ERROR: queue overflow\n");
    exit(1);
  }
  queue[QWRAP(qbeg + i)].coord = coord;
  queue[QWRAP(qbeg + i)].msg = msg;
  qbeg = QWRAP(qbeg+1);
}

ptr dequeue(){
  int i;
  for(i=0;i<QLENGTH;i++){
    if(QWRAP(qend+i) == qbeg)
      break;
    if(queue[QWRAP(qend+i)].msg.hdr != NIL){
      qend = QWRAP(qend+i+1);
      return queue[QWRAP(qend-1)].msg;
    }
  }
}

void show_ptr(ptr ptr){
  switch(ptr.hdr){
  case NIL:     printf("_"); break;
  case LOCAL:   printf("^"); break;
  case INTEGER: printf("i"); break;
  case SYMBOL:  printf("s"); break;
  default:      printf("l"); break;
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
        printf("["); show_ptr(tmp.msg);
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
  coord free;
  free.x = place.x;
  free.y = place.y;
  int cntrl, index;
  /* cycle around original place
   * ===========================
   *        ^       1
   *        |    ^ --> |
   *       3|   1|     |2
   *        | <------- v
   *               2
   */
  cntrl = 2;
  for(index=1; index<SIZE*SIZE; index++){
    printf("%d %d %d %d [%d]\n",
           index, cntrl, (cntrl/2), (index % (cntrl/2)),
           AT(free).refs);
    if((index % (cntrl/2)) == 0) {
      cntrl++;
    } else {
      switch(cntrl % 4){
      case 1: free.y = WRAP(free.y + 1); break;
      case 2: free.x = WRAP(free.x + 1); break;
      case 3: free.y = WRAP(free.y - 1); break;
      case 4: free.x = WRAP(free.x - 1); break;
      }
    }
    if(AT(free).refs == 0) break;
  }
  if(index == (SIZE*SIZE-1)) {
    printf("ERROR: exhausted world\n");
    exit(1);
  } else {
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
  default: to = from; break; /* LAMBDA */
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
  case SYMBOL: if(to.car != var) break;
  case LOCAL:
  default:
    if(to.hdr > 3 && to.hdr == var) break; /* LAMBDA shadow */
    duplicate_ptr(new, to, new_coord, to_coord); break;
  }
}

void run(coord place){
  coord tmp_coord;
  switch(AT(place).msg.hdr){
  case INTEGER:
    AT(place).refs += AT(place).msg.car;
    if(AT(place).car.hdr == LOCAL){
      tmp_coord.x = AT(place).car.car;
      tmp_coord.y = AT(place).car.cdr;
      enqueue(tmp_coord, AT(place).msg);
    }
    if(AT(place).cdr.hdr == LOCAL){
      tmp_coord.x = AT(place).cdr.car;
      tmp_coord.y = AT(place).cdr.cdr;
      enqueue(tmp_coord, AT(place).msg);
    }
    break;
  default: /* LAMBDA application */ break;
  }
  AT(place).msg.hdr = NIL;
}

int main(int argc, char* argv){
  int i;
  coord place, new;
  place.x = place.y = 2;
  AT(place).car.hdr = 32;       /* LAMBDA 32 */
  AT(place).cdr.hdr = 1;        /* LOCAL */
  AT(place).cdr.car = 1;
  AT(place).cdr.cdr = 0;
  place.x = 3;
  AT(place).car.hdr = 3;        /* SYMBOL */
  AT(place).car.car = 32;       /* 32 */
  for(i=0;i<90;i++){
    show_world();
    sleep(1);
    new = open_space(place);
    AT(place).refs = 1;
  }
  /* TODO: loop through oncs in random order calling run */
  /* TODO: scan through message applying them to oncs with empty msg slots */
  return 0;
}
