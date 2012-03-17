#include <stdlib.h>
#include <stdio.h>
#include "oncs.h"

onc world[SIZE][SIZE];
msg queue[QLENGTH];
int qbeg, qend = 0;

void enqueue(msg msg){
  int i;
  for(i=0;i<QLENGTH;i++)
    if(queue[QWRAP(qend + i)].mcar.hdr == NIL)
      break;
  if(i == (QLENGTH - 1)){
    printf("ERROR: queue overflow\n");
    exit(1);
  }
  queue[QWRAP(qend + i)] = msg;
  qend = QWRAP(qend+1);
}

msg dequeue(){
  int tmp = qbeg;
  if(queue[tmp].mcar.hdr != NIL){
    qbeg = QWRAP(qbeg+1);
    return queue[tmp];
  } else {
    printf("ERROR: queue underflow\n");
    exit(1);
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

void duplicate(coord to, coord from){
  coord tmp_from, tmp_to;
  AT(to) = AT(from);
  if(AT(to).car.hdr == LOCAL){
    tmp_from.x = AT(to).car.car;
    tmp_from.y = AT(to).car.cdr;
    tmp_to = open_space(to);
    duplicate(tmp_to, tmp_from);
  }
  if(AT(to).cdr.hdr == LOCAL){
    tmp_from.x = AT(to).cdr.car;
    tmp_from.y = AT(to).cdr.cdr;
    tmp_to = open_space(to);
    duplicate(tmp_to, tmp_from);
  }
}

void replace_ptr(int var, ptr to, coord to_coord, ptr new, coord new_coord){
  switch(to.hdr){
  case NIL:
  case INTEGER: break;
  case SYMBOL: if(to.car != var) break;
  case LAMBDA: if(to.hdr == var) break;
  case LOCAL:  /* TODO: duplicate locally */ break;
  }
}

void run(coord place){
  msg msg;
  switch(AT(place).mcar.hdr){
  case INTEGER:                 /* update number of references */
    AT(place).refs += AT(place).mcar.car;
    msg.mcar = AT(place).mcar;
    msg.mcdr = AT(place).mcdr;
    if(AT(place).car.hdr == LOCAL){
      msg.coord.x = AT(place).car.car;
      msg.coord.y = AT(place).car.cdr;
    }
    if(AT(place).cdr.hdr == LOCAL){
      msg.coord.x = AT(place).cdr.car;
      msg.coord.y = AT(place).cdr.cdr;
    }
    enqueue(msg);
    break;
  default: /* LAMBDA application */ break;
  }
  AT(place).mcar.hdr = NIL;
}
