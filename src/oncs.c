#include <stdlib.h>
#include <stdio.h>
#include <oncs.h>

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

coord open_space(coord place){
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
  coord t1, t2;
  AT(to) = AT(from);
  AT(to).refs = 1;
  AT(to).mcar.hdr = AT(to).mcdr.hdr = NIL;
  if(AT(to).car.hdr == LOCAL) DUPLICATE_LOCAL(AT(to).car, to, t1, t2);
  if(AT(to).cdr.hdr == LOCAL) DUPLICATE_LOCAL(AT(to).cdr, to, t1, t2);
}

void run_queue(){
  if(queue[qbeg].mcar.hdr != NIL){
    msg msg = dequeue();
    if(AT(msg.coord).mcar.hdr == NIL){
      AT(msg.coord).mcar = msg.mcar;
      AT(msg.coord).mcdr = msg.mcdr;
    } else {
      enqueue(msg);
    }
  }
}

void run(coord place){
  coord t1, t2;
  msg msg;
  switch(AT(place).mcar.hdr){
  case NIL: /* waiting loop */
    if(AT(place).car.hdr == LOCAL){
      t1.x = AT(place).car.car;
      t1.y = AT(place).car.cdr;
      if(AT(t1).car.hdr == LAMBDA){
        msg.coord = t1;
        msg.mcar.hdr = LAMBDA;
        msg.mcar.car = 0;
        msg.mcdr = AT(place).cdr;
        enqueue(msg);
      }
    }
  case LOCAL: case SYMBOL: /* undefined */ break;
  case INTEGER: /* update number of references */
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
  case LAMBDA: /* perform lambda application */
    LAMBDA_APP(AT(place).car, msg, t1, t2);
    LAMBDA_APP(AT(place).cdr, msg, t1, t2);
    break;
  }
  AT(place).mcar.hdr = NIL;
}
