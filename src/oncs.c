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
    ERROR("queue overflow");
  queue[QWRAP(qend + i)] = msg;
  qend = QWRAP(qend+1);
}

msg dequeue(){
  int tmp = qbeg;
  if(queue[tmp].mcar.hdr != NIL){
    qbeg = QWRAP(qbeg+1);
    return queue[tmp];
  } else ERROR("queue underflow");
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
  if (tried >= SIZE*SIZE) { ERROR("exhausted world"); }
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
      DEBUG2("message requeue: (%d,%d)\n", msg.coord.x, msg.coord.y);
      enqueue(msg);
    }
  }
}

void update_ref_msg(coord place, int diff){
  msg msg;
  msg.coord = place;
  msg.mcar.hdr = INTEGER;
  msg.mcar.car = diff;
  DEBUG2("update ref (%d,%d)\n", place.x, place.y);
  enqueue(msg);
}

void run(coord place){
  msg msg;
  coord t1, t2, t3;
  int i1;
  DEBUG2("running(%d,%d)\n", place.x, place.y);
  switch(AT(place).mcar.hdr){
  case NIL: /* waiting loop */
    if(AT(place).cdr.hdr == LOCAL){
      COORD_OF_PTR(t2, AT(place).cdr);
      if(AT(t2).car.hdr == LOCAL)
        COORD_OF_PTR(t3, AT(t2).car);
    }
    switch(AT(place).car.hdr){
    case LOCAL:
      /* point t1 at car */
      COORD_OF_PTR(t1, AT(place).car);
      /* apply car to cdr and replace self with result */
      switch(AT(t1).car.hdr){
      case LAMBDA:
        if((AT(t1).cdr.hdr != LOCAL) && (AT(t1).cdr.hdr != NIL))
          ERROR("cdr (body) of LAMBDA must be LOCAL or NIL");
        /* build lambda message from car and send to myself */
        msg.coord = place;
        msg.mcar.hdr = LAMBDA;
        msg.mcar.car = AT(t1).car.car;
        msg.mcdr = AT(t2).car;
        if(AT(place).cdr.hdr == LOCAL && AT(t2).car.hdr == LOCAL)
          update_ref_msg(t3, 1);
        DEBUG2("NIL: l->me (%d,%d)\n", msg.coord.x, msg.coord.y);
        enqueue(msg);
        if(AT(place).cdr.hdr == LOCAL){
          AT(place).cdr = AT(t2).cdr;
          if(AT(t2).car.hdr == LOCAL) update_ref_msg(t3, -1);
        }
        /* replace self with body of lambda expression (cadr) */
        AT(place).car = AT(t1).cdr;
        /* remove lambda expression */
        DEBUG2("NIL: removing car of l at (%d,%d)\n", t1.x, t1.y);
        AT(t1).refs = 0;
        DEBUG2("NIL: removing cdr of l at (%d,%d)\n", t2.x, t2.y);
        AT(t2).refs = 0;
        break;
      case UNPACK: UNPACK_APP(AT(place).car, t1); break;
      default:
        /* if not lambda or unpack then let it run */
        DEBUG2("NIL: running car at (%d,%d)\n", t1.x, t1.y);
        run(t1);
      }
      break;
    case PRIMOPT: /* curry up primitive operation */
      if(AT(place).cdr.hdr == INTEGER){
        PRIMOPT_APP(AT(place).car, AT(place).cdr.car);
      } else if (AT(place).cdr.hdr == LOCAL) {
        if(AT(t2).car.hdr == INTEGER) {
          AT(place).cdr = AT(t2).cdr;
          if(AT(t2).cdr.hdr == LOCAL){
            COORD_OF_PTR(t1, AT(t2).cdr);
            update_ref_msg(t1, 1);
          }
          PRIMOPT_APP(AT(place).car, AT(t2).car.car);
          update_ref_msg(t2, -1);
        } else {
          DEBUG2("NIL:PRIMOPT running cdr at (%d,%d)\n", t2.x, t2.y);
          run(t2);
        }
      }
      break;
    case CURRIED: /* apply curried primitive operation */
      if(AT(place).cdr.hdr == INTEGER){
        CURRIED_APP(AT(place), AT(place).cdr);
      } else if (AT(place).cdr.hdr == LOCAL) {
        COORD_OF_PTR(t2, AT(place).cdr);
        if(AT(t2).car.hdr == INTEGER) {
          AT(place).cdr = AT(t2).cdr;
          if(AT(t2).cdr.hdr == LOCAL){
            COORD_OF_PTR(t1, AT(t2).cdr);
            update_ref_msg(t1, 1);
          }
          CURRIED_APP(AT(place), AT(t2).car);
          update_ref_msg(t2, -1);
        } else {
          COORD_OF_PTR(t2, AT(place).cdr);
          DEBUG2("NIL:CURRIED running cdr at (%d,%d)\n", t2.x, t2.y);
          run(t2);
        }
      }
      break;
    }
    if(AT(place).cdr.hdr == LOCAL) {
      if(AT(t2).car.hdr == UNPACK) { UNPACK_APP(AT(place).cdr, t2); }
      else                         { run(t2); }
    }
    if(AT(place).car.hdr == BOOLEAN)
      BOOLEAN_APP(place, AT(place).car, t1, t2, i1);
    if(AT(place).cdr.hdr == BOOLEAN)
      BOOLEAN_APP(place, AT(place).cdr, t1, t2, i1);
    break;
  case LOCAL: case SYMBOL: case BOOLEAN: /* undefined */ break;
  case INTEGER: /* update number of references */
    AT(place).refs += AT(place).mcar.car;
    msg.mcar = AT(place).mcar;
    if(AT(place).car.hdr == LOCAL){
      COORD_OF_PTR(msg.coord, AT(place).car);
      DEBUG2("INTEGER: update car ref at (%d,%d)\n",
             msg.coord.x, msg.coord.y);
      enqueue(msg);
    }
    if(AT(place).cdr.hdr == LOCAL){
      COORD_OF_PTR(msg.coord, AT(place).cdr);
      DEBUG2("INTEGER: update cdr ref at (%d,%d)\n",
             msg.coord.x, msg.coord.y);
      enqueue(msg);
    }
    break;
  case LAMBDA:  /* perform lambda application */
    /* TODO: what happens here if the replacer is not a pointer but
       is a literal value  */
    if(AT(place).mcdr.hdr == LOCAL) COORD_OF_PTR(t2, AT(place).mcdr);
    LAMBDA_APP(place, AT(place).car, msg, t1);
    LAMBDA_APP(place, AT(place).cdr, msg, t1);
    if(AT(place).mcdr.hdr == LOCAL) update_ref_msg(t2, -1);
    break;
  }
  AT(place).mcar.hdr = NIL;
}
