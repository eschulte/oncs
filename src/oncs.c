/* Copyright (C) 2012 Eric Schulte */
#include <stdlib.h>
#include <oncs.h>

onc world[SIZE][SIZE];
msg queue[QLENGTH];
int qbeg, qend = 0;

void enqueue(msg msg){
  DEBUG2("enqueue(%d,%d)\n", msg.coord.x, msg.coord.y);
  DEBUG3("enqueue msg.ptr=(%d,%d,%d)\n",
         msg.mcar.hdr, msg.mcar.car, msg.mcar.cdr);
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
  DEBUG2("enqueue from update ref (%d,%d)\n", place.x, place.y);
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

ptr lambda_app(msg l_msg, ptr ptr){
  switch(ptr.hdr){
  case NIL:
  case INTEGER:
  case BOOLEAN:
  case LAMBDA: break;
  case SYMBOL:
    if(l_msg.mcar.car == ptr.car)
      ptr = copy_ptr(l_msg.mcdr);
    break;
  case LOCAL:
    COORD_OF_PTR(l_msg.coord, ptr);
    l_msg.mcdr = copy_ptr(l_msg.mcdr);
    DEBUG("enqueue from LAMBDA_APP\n");
    enqueue(l_msg);
    break;
  }
  return ptr;
}

void app_abs(coord place){
  msg msg;
  coord c_car, c_cdr;
  if(AT(place).car.hdr != LOCAL)
    ERROR("malformed app_abs");
  COORD_OF_PTR(c_car, AT(place).car);
  if(AT(c_car).car.hdr != LAMBDA ||
     (AT(c_car).cdr.hdr != LOCAL &&
      AT(c_car).cdr.hdr != NIL))
    ERROR("malformed app_abs.car");
  /* update AT(place).car */
  AT(place).car = copy_ptr(AT(c_car).cdr);
  /* update AT(place).cdr */
  if(AT(place).cdr.hdr == LOCAL){
    COORD_OF_PTR(c_cdr, AT(place).cdr);
    msg.mcdr = copy_ptr(AT(c_cdr).car);
    AT(place).cdr = copy_ptr(AT(c_cdr).cdr);
  } else {
    msg.mcdr = AT(place).cdr;
    AT(place).cdr.hdr = NIL;
  }
  AT(place).cdr = copy_ptr(AT(c_car).cdr);
  /* apply to AT(place).car */
  msg.mcar.hdr = LAMBDA;
  msg.mcar = copy_ptr(AT(c_car).car);
  AT(place).car = lambda_app(msg, AT(place).car);
}

void run(coord place){
  msg msg;
  coord c1, c2;
  switch(AT(place).mcar.hdr){
  case NIL:                     /* waiting loop */
    switch(AT(place).car.hdr){
    case LOCAL:
      COORD_OF_PTR(c1, AT(place).car);
      switch(AT(c1).car.hdr){
      case LAMBDA: app_abs(place); break;
      case PRIMOPT: break;
      }
      break;
    case PRIMOPT: break;
    case CURRIED: break;
    }
    break;
  case INTEGER:                 /* update number of references */
    AT(place).refs += AT(place).mcar.car;
    msg.mcar = AT(place).mcar;
    INTEGER_APP(AT(place).car, msg);
    INTEGER_APP(AT(place).cdr, msg);
    break;
  case LAMBDA:                  /* perform lambda application */
    msg.mcar = AT(place).mcar; msg.mcdr = AT(place).mcdr;
    if(AT(place).mcdr.hdr == LOCAL) COORD_OF_PTR(c2, AT(place).mcdr);
    LAMBDA_APP(msg, AT(place).car, c1);
    LAMBDA_APP(msg, AT(place).cdr, c1);
    if(AT(place).mcdr.hdr == LOCAL) update_ref_msg(c2, -1);
    break;
  }
  AT(place).mcar.hdr = NIL;
}
