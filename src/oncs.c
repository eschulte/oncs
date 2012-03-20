/* Copyright (C) 2012 Eric Schulte */
#include <stdlib.h>
#include <stdio.h>
#include <oncs.h>

onc world[SIZE][SIZE];
msg queue[QLENGTH];
int qbeg, qend = 0;

void enqueue(msg msg){
  DEBUG2("enqueue(%d,%d)\n", msg.coord.x, msg.coord.y);
  int i;
  for(i=0;i<QLENGTH;i++)
    if(queue[QWRAP(qend + i)].mcar.hdr == NIL)
      break;
  if(i == (QLENGTH - 1))
    ERROR("queue overflow\n");
  queue[QWRAP(qend + i)] = msg;
  qend = QWRAP(qend+1);
}

msg dequeue(){
  int tmp = qbeg;
  if(queue[tmp].mcar.hdr != NIL){
    qbeg = QWRAP(qbeg+1);
    return queue[tmp];
  } else ERROR("queue underflow\n");
}

void clear_world(){
  int i,j;
  for(i=0;i<SIZE;i++)
    for(j=0;j<SIZE;j++)
      world[i][j].refs = 0;
}

void clear_queue(){
  int i;
  for(i=0;i<QLENGTH;i++)
    queue[i].mcar.hdr = NIL;
  qbeg = qend = 0;
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
  if (tried >= SIZE*SIZE) { ERROR("exhausted world\n"); }
  else { return place; }
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

void update_ref_msg(coord place, int diff){
  msg msg;
  msg.coord = place;
  msg.mcar.hdr = INTEGER;
  msg.mcar.car = diff;
  DEBUG("update ref\n");
  enqueue(msg);
}

void run(coord place){
  coord t1, t2;
  msg msg;
  DEBUG2("running(%d,%d)\n", place.x, place.y);
  switch(AT(place).mcar.hdr){
  case NIL: /* waiting loop */
    if(AT(place).car.hdr == LOCAL){
      /* point t1 at abstraction */
      COORD_OF_PTR(t1, AT(place).car);
      /* apply car to cdr and replace self with result */
      if(AT(t1).car.hdr == LAMBDA){
        /* build lambda message from car and send to myself */
        msg.coord = place;
        msg.mcar.hdr = LAMBDA;
        msg.mcar.car = AT(t1).car.car;
        msg.mcdr = AT(place).cdr;
        if(AT(place).cdr.hdr == LOCAL){
          /* point t2 at second half of application */
          COORD_OF_PTR(t2, AT(place).cdr);
          update_ref_msg(t2, 1);
        }
        enqueue(msg);
        /* self no longer points to cdr */
        if(AT(place).cdr.hdr == LOCAL){
          DEBUG2("NIL update ref to (%d,%d)\n", t2.x, t2.y);
          update_ref_msg(t2, -1);
        }
        /* replace self with body of lambda expression (cadr) */
        t2.x = AT(t1).cdr.car; t2.y = AT(t1).cdr.cdr;
        AT(place).car = AT(t2).car;
        AT(place).cdr = AT(t2).cdr;
        /* remove lambda expression */
        AT(t1).refs = 0;
        AT(t2).refs = 0;
      } else { run(t1); }
    }
    if(AT(place).cdr.hdr == LOCAL){
      COORD_OF_PTR(t1, AT(place).cdr);
      run(t1);
    }
    break;
  case LOCAL: case SYMBOL: /* undefined */ break;
  case INTEGER: /* update number of references */
    AT(place).refs += AT(place).mcar.car;
    msg.mcar = AT(place).mcar;
    if(AT(place).car.hdr == LOCAL){
      COORD_OF_PTR(msg.coord, AT(place).car);
      enqueue(msg);
    }
    if(AT(place).cdr.hdr == LOCAL){
      COORD_OF_PTR(msg.coord, AT(place).cdr);
      DEBUG("INTEGER2\n");
      enqueue(msg);
    }
    break;
  case LAMBDA: /* perform lambda application */
    t1.x = AT(place).mcdr.car; t1.y = AT(place).mcdr.cdr;
    LAMBDA_APP(AT(place).car, msg, t1, t2);
    LAMBDA_APP(AT(place).cdr, msg, t1, t2);
    DEBUG2("LAMBDA update ref to (%d,%d)\n", t1.x, t1.y);
    update_ref_msg(t1, -1);
    break;
  }
  AT(place).mcar.hdr = NIL;
}
