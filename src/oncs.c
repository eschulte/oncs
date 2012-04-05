/* Copyright (C) 2012 Eric Schulte */
#include <stdlib.h>
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

ptr copy_ptr(ptr ptr){
  coord temp;
  if(ptr.hdr == LOCAL){
    COORD_OF_PTR(temp, ptr);
    update_ref_msg(temp, 1);
  }
  return ptr;
}

ptr delete_ptr(ptr ptr){
  coord temp;
  if(ptr.hdr == LOCAL){
    COORD_OF_PTR(temp, ptr);
    update_ref_msg(temp, -1);
  }
  return ptr;
}

void app_abs(coord place){
  if(AT(place).car.hdr != LOCAL)
    ERROR("malformed app_abs");
  coord l;
  COORD_OF_PTR(l, AT(place).car);
  if(AT(l).car.hdr != LAMBDA ||
     (AT(l).car.hdr != LOCAL &&
      AT(l).car.hdr != NIL))
    ERROR("malformed app_abs.car");
  msg msg;
  msg.mcar = copy_ptr(AT(l).car);
  msg.mcdr = copy_ptr(AT(place).cdr);
  /* TODO: where does this point */
  msg.coord;
}

void run(coord place){
  
}
