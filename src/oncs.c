/* Copyright (C) 2012 Eric Schulte */
#include <stdlib.h>
#include <oncs.h>

onc world[SIZE][SIZE];
msg queue[QLENGTH];
int qbeg, qend = 0;

void enqueue(msg msg){
  DEBUG2("enqueue(%d,%d) ", msg.coord.x, msg.coord.y);
  switch(msg.mcar.hdr){
  case LOCAL:
    DEBUG2("LOCAL:(%d,%d)", msg.mcar.car, msg.mcar.cdr);
    break;
  case INTEGER:
    DEBUG1("INTEGER:(%d)", msg.mcar.car);
    break;
  case LAMBDA:
    DEBUG1("LAMBDA:(%d)", msg.mcar.car);
    break;
  default:
    DEBUG("OTHER");
    break;
  }
  DEBUG("\n");
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
    for(j=0;j<SIZE;j++) {
      world[i][j].refs = 0;
      /* world[i][j].car.hdr = 0; */
      /* world[i][j].cdr.hdr = 0; */
    }
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

int run_queue(){
  if(queue[qbeg].mcar.hdr != NIL){
    msg msg = dequeue();
    if(AT(msg.coord).mcar.hdr == NIL){
      AT(msg.coord).mcar = msg.mcar;
      AT(msg.coord).mcdr = msg.mcdr;
      return 1;
    } else {
      run(msg.coord);
      DEBUG2("message requeue: (%d,%d)\n", msg.coord.x, msg.coord.y);
      enqueue(msg);
      return 0;
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
    DEBUG("update_ref_msg from copy_ptr\n");
    update_ref_msg(temp, 1);
  }
  return ptr;
}

ptr delete_ptr(ptr ptr){
  coord temp;
  if(ptr.hdr == LOCAL){
    COORD_OF_PTR(temp, ptr);
    DEBUG("update_ref_msg from delete_ptr\n");
    update_ref_msg(temp, -1);
  }
  return ptr;
}

ptr replace_ptr(ptr old, ptr new){
  new = copy_ptr(new);
  delete_ptr(old);
  return new;
}

ptr duplicate_ptr(ptr ptr, int refs){
  coord orig, new;
  if(ptr.hdr == LOCAL){
    COORD_OF_PTR(orig, ptr);
    new = open_space(orig);
    AT(new).refs = refs;
    ptr.car = new.x; ptr.cdr = new.y;
    AT(new).car = duplicate_ptr(AT(orig).car, refs);
    AT(new).cdr = duplicate_ptr(AT(orig).cdr, refs);
  }
  return ptr;
}

int ptr_to_string(ptr ptr, char *buf, int index, int car_p){
  int i, j;
  char c;
  char s[20];
  coord coord;
  switch(ptr.hdr){
  case UNPACK: buf[index] = '~'; index++; break;
  case NIL: break;
  case LOCAL:
    coord.x = ptr.car; coord.y = ptr.cdr;
    if(car_p) { buf[index] = '('; index++; }
    index = ptr_to_string(AT(coord).car, buf, index, 1);
    buf[index] = ' '; index++;
    index = ptr_to_string(AT(coord).cdr, buf, index, 0);
    if(car_p) { buf[index] = ')'; index++; }
    return index;
  case LAMBDA:
    buf[index] = '#'; index++;
    buf[index] = 'L'; index++;
  case SYMBOL:
    if(ptr.hdr != LAMBDA){
      buf[index] = '#'; index++;
      buf[index] = 'S'; index++;
    }
  case INTEGER:
    i = sprintf(s, "%d", ptr.car);
    for(j=0;j<i;j++){
      buf[index] = s[j];
      index++;
    }
    break;
  case PRIMOPT:
    switch(ptr.car){
    case PLUS:   buf[index] = '+'; break;
    case MINUS:  buf[index] = '-'; break;
    case TIMES:  buf[index] = '*'; break;
    case DIVIDE: buf[index] = '/'; break;
    case EQUAL:  buf[index] = '='; break;
    case LESS:   buf[index] = '<'; break;
    }
    index++;
    break;
  case CURRIED:
    switch(ptr.car){
    case PLUS:   c = '+'; break;
    case MINUS:  c = '-'; break;
    case TIMES:  c = '*'; break;
    case DIVIDE: c = '/'; break;
    case EQUAL:  c = '='; break;
    case LESS:   c = '<'; break;
    }
    i = sprintf(s, "%c%d", c, ptr.cdr);
    for(j=0;j<i;j++){
      buf[index] = s[j];
      index++;
    }
    break;
  case BOOLEAN:
    switch(ptr.car){
    case TRUE:  buf[index] = 't'; break;
    case FALSE: buf[index] = 'f'; break;
    }
    index++;
    break;
  }
  return index;
}

int onc_to_string(coord place, char *buf, int index){
  buf[index] = '('; index++;
  index = ptr_to_string(AT(place).car, buf, index, 1);
  buf[index] = ' '; index++;
  index = ptr_to_string(AT(place).cdr, buf, index, 0);
  buf[index] = ')'; index++;
  buf[index] = '\0';
  return index;
}

ptr lambda_app(msg l_msg, ptr ptr, int refs){
  coord coord;
  switch(ptr.hdr){
  case NIL:
  case INTEGER:
  case BOOLEAN:
  case LAMBDA: break;
  case SYMBOL:
    if(l_msg.mcar.car == ptr.car)
      ptr = duplicate_ptr(l_msg.mcdr, refs);
    break;
  case LOCAL:
    COORD_OF_PTR(l_msg.coord, ptr);
    l_msg.mcdr = copy_ptr(l_msg.mcdr);
    DEBUG("enqueue from LAMBDA_APP\n");
    enqueue(l_msg);
    break;
  }
  delete_ptr(l_msg.mcdr);
  return ptr;
}

void app_abs(coord place){
  char str[4000];
  msg msg;
  coord c_car, c_cdr;
  /* setup */
  if(AT(place).car.hdr != LOCAL)
    ERROR("malformed app_abs");
  COORD_OF_PTR(c_car, AT(place).car);
  if(AT(c_car).car.hdr != LAMBDA ||
     (AT(c_car).cdr.hdr != LOCAL &&
      AT(c_car).cdr.hdr != NIL))
    ERROR("malformed app_abs.car");
  if(AT(place).cdr.hdr == LOCAL)
    COORD_OF_PTR(c_cdr, AT(place).cdr);
  /* don't apply to nil or ends of lists */
  if(AT(place).cdr.hdr != NIL){
    onc_to_string(place, str, 0);
    DEBUG3("app_abs(%d,%d) %s\n", place.x, place.y, str);
    /* 1. make new message */
    msg.mcar.hdr = LAMBDA;
    /* 2. copy λ1 to msg.car */
    msg.mcar = copy_ptr(AT(c_car).car);
    if(AT(place).cdr.hdr == LOCAL){
      /* 3. copy a to msg.cdr */
      msg.mcdr = copy_ptr(AT(c_cdr).car);
      /* 4. replace 2 with 4 */
      AT(place).cdr = replace_ptr(AT(place).cdr, AT(c_cdr).cdr);
    } else {
      msg.mcdr = copy_ptr(AT(place).cdr);
      AT(place).cdr.hdr = NIL;
    }
    /* 5. replace 1 with 9 (or with 9's target) */
    if(0 && AT(c_car).cdr.hdr == LOCAL){
      COORD_OF_PTR(c_cdr, AT(c_car).cdr);
      AT(place).car = replace_ptr(AT(place).car, AT(c_cdr).car);
      AT(place).cdr = replace_ptr(AT(place).car, AT(c_cdr).car);
    } else {
      AT(place).car = replace_ptr(AT(place).car, AT(c_car).cdr);
    }
    /* 6. msg goes to 1 */
    AT(place).car = lambda_app(msg, AT(place).car, AT(place).refs);
  }
}

void run(coord place){
  msg msg;
  coord c1, c2;
  int i1;
  switch(AT(place).mcar.hdr){
  case NIL:                     /* waiting loop */
    switch(AT(place).car.hdr){
    case LOCAL:
      COORD_OF_PTR(c1, AT(place).car);
      switch(AT(c1).car.hdr){
      case LAMBDA: app_abs(place); break;
      case UNPACK:
        AT(place).car = replace_ptr(AT(place).car, AT(c1).cdr);
        break;
      default: run(c1); break;
      }
      break;
    case PRIMOPT:
      switch(AT(place).cdr.hdr){
      case INTEGER:
        AT(place).car.cdr = AT(place).cdr.car;
        AT(place).car.hdr = CURRIED;
        break;
      case LOCAL:
        COORD_OF_PTR(c2, AT(place).cdr);
        if(AT(c2).car.hdr == INTEGER){
          AT(place).car.cdr = AT(c2).car.car;
          AT(place).car.hdr = CURRIED;
          AT(place).cdr = replace_ptr(AT(place).cdr, AT(c2).cdr);
        } else run(c2);
        break;
      }
      break;
    case CURRIED:
      switch(AT(place).cdr.hdr){
      case INTEGER:
        CURRIED_APP(AT(place), AT(place).cdr);
        break;
      case LOCAL:
        COORD_OF_PTR(c2, AT(place).cdr);
        if(AT(c2).car.hdr == INTEGER){
          if(AT(c2).cdr.hdr != NIL)
            ERROR("CURRIED primitive operations take only 1 arg");
          CURRIED_APP(AT(place), AT(c2).car);
          update_ref_msg(c2, -1);
        } else run(c2);
        break;
      }
      break;
    }
    if(AT(place).car.hdr == BOOLEAN)
      BOOLEAN_APP(place, AT(place).car, c1, c2, i1);
    if(AT(place).cdr.hdr == BOOLEAN)
      BOOLEAN_APP(place, AT(place).cdr, c1, c2, i1);
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
    AT(place).car = lambda_app(msg, AT(place).car, AT(place).refs);
    /* don't descend down shadowing lambdas */
    if(! (AT(place).car.hdr == LAMBDA &&
          AT(place).car.car == msg.mcar.car)) {
      copy_ptr(msg.mcdr);
      AT(place).cdr = lambda_app(msg, AT(place).cdr, AT(place).refs);
    }
    DEBUG("update_ref_msg from run:lambda_application\n");
    if(AT(place).mcdr.hdr == LOCAL) update_ref_msg(c2, -1);
    break;
  }
  AT(place).mcar.hdr = NIL;
}
