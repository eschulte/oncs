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

void clear_place(coord place){
  AT(place).refs = 0;
  AT(place).mcar.hdr = 0;
  AT(place).mcar.car = 0;
  AT(place).mcar.cdr = 0;
  AT(place).mcdr.hdr = 0;
  AT(place).mcdr.car = 0;
  AT(place).mcdr.cdr = 0;
  AT(place).car.hdr = 0;
  AT(place).car.car = 0;
  AT(place).car.cdr = 0;
  AT(place).cdr.hdr = 0;
  AT(place).cdr.car = 0;
  AT(place).cdr.cdr = 0;
}

void clear_world(){
  int i,j;
  coord place;
  for(i=0;i<SIZE;i++)
    for(j=0;j<SIZE;j++) {
      place.x = i; place.y = j;
      world[i][j].refs = 0;
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
  else { clear_place(place); return place; }
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
    DEBUG2("update_ref_msg(%d,%d) from copy_ptr\n",
           temp.x, temp.y);
    update_ref_msg(temp, 1);
  }
  return ptr;
}

ptr delete_ptr(ptr ptr){
  coord temp;
  if(ptr.hdr == LOCAL){
    COORD_OF_PTR(temp, ptr);
    DEBUG2("update_ref_msg(%d,%d) from delete_ptr\n",
           temp.x, temp.y);
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

int value_p(ptr ptr){
  coord place, place2;
  switch(ptr.hdr){
  case NIL: return FALSE; break;
  case LOCAL:
    /* lambda abstraction is value if can't be applied */
    COORD_OF_PTR(place, ptr);
    DEBUG2("-> (%d,%d)", place.x, place.y);
    switch(AT(place).car.hdr){
    case LAMBDA:
      DEBUG3(".cdr(%d,%d,%d) ",
             AT(place).cdr.hdr, AT(place).cdr.car, AT(place).cdr.cdr);
      return ! value_p(AT(place).cdr);
      break;
    case LOCAL:
      /* if lambda pointing to nil return TRUE */
      if(AT(place).cdr.hdr == NIL){
        COORD_OF_PTR(place2, AT(place).car);
        if(AT(place2).car.hdr == LAMBDA){
          return TRUE; break;
        }
      }
    default:
      DEBUG3(".car(%d,%d,%d) ",
             AT(place).car.hdr, AT(place).car.car, AT(place).car.cdr);
      return value_p(AT(place).car);
      break;
    }
    break;
  default: return TRUE; break;
  }
}

int ptr_to_string(ptr ptr, char *buf, int index, int car_p){
  int i, j;
  char c;
  char s[20];
  coord coord;
  switch(ptr.hdr){
  case UNPACK: buf[index] = '~'; index++; break;
  case NIL: index--; break;
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
    if(ptr.cdr) buf[index] = 'l';
    else        buf[index] = 'L';
    index++;
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
  index = ptr_to_string(AT(place).car, buf, index, 1);
  buf[index] = ' '; index++;
  index = ptr_to_string(AT(place).cdr, buf, index, 0);
  buf[index] = '\0';
  return index;
}

ptr lambda_app(msg l_msg, ptr ptr, int refs){
  coord coord;
  switch(ptr.hdr){
  case NIL:
  case INTEGER:
  case BOOLEAN: break;
  case LAMBDA:
    /* lock lambda if message is locked */
    DEBUG3("L%d locking %d->%d\n",
           ptr.car, ptr.cdr, l_msg.mcar.cdr);
    ptr.cdr = l_msg.mcar.cdr;
    break;
  case SYMBOL:
    if(l_msg.mcar.car == ptr.car)
      ptr = duplicate_ptr(l_msg.mcdr, refs);
    break;
  case LOCAL:
    COORD_OF_PTR(l_msg.coord, ptr);
    DEBUG2("copy_ptr(%d,%d) from lambda_app\n",
           l_msg.mcdr.car, l_msg.mcdr.cdr);
    l_msg.mcdr = copy_ptr(l_msg.mcdr);
    DEBUG("enqueue from LAMBDA_APP\n");
    enqueue(l_msg);
    break;
  }
  delete_ptr(l_msg.mcdr);
  return ptr;
}

void app_abs(coord place){
  ptr ptr;
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
  /* don't apply to non values -- call-by-value */
  if(value_p(AT(place).cdr)){
    /* 1. make new message */
    msg.mcar.hdr = LAMBDA;
    /* 2. copy λ1 to msg.car */
    msg.mcar = copy_ptr(AT(c_car).car);
    /* 3. copy a to msg.cdr */
    if(AT(place).cdr.hdr == LOCAL)
      msg.mcdr = copy_ptr(AT(c_cdr).car);
    else
      msg.mcdr = copy_ptr(AT(place).cdr);
    /* 4. replace (1,2) with (9,10) */
    if(AT(c_car).cdr.hdr == LOCAL){
      COORD_OF_PTR(c_car, AT(c_car).cdr);
      DEBUG8("(%d,%d) replace_ptr(%d,%d,%d,)(%d,%d,%d)"
             "-replace (1,2) with (9,10)\n",
             place.x, place.y,
             AT(place).car.hdr, AT(place).car.car, AT(place).car.cdr,
             AT(c_car).car.hdr, AT(c_car).car.car, AT(c_car).car.cdr);
      AT(place).car = replace_ptr(AT(place).car, AT(c_car).car);
      DEBUG8("(%d,%d) replace_ptr(%d,%d,%d,)(%d,%d,%d)"
             "-replace (1,2) with (9,10)\n",
             place.x, place.y,
             AT(place).cdr.hdr, AT(place).cdr.car, AT(place).cdr.cdr,
             AT(c_car).cdr.hdr, AT(c_car).cdr.car, AT(c_car).cdr.cdr);
      AT(place).cdr = replace_ptr(AT(place).cdr, AT(c_car).cdr);
    } else {
      DEBUG8("(%d,%d) replace_ptr(%d,%d,%d,)(%d,%d,%d)"
             "-replace (1,2) with (9,10)\n",
             place.x, place.y,
             AT(place).car.hdr, AT(place).car.car, AT(place).car.cdr,
             AT(c_car).cdr.hdr, AT(c_car).cdr.car, AT(c_car).cdr.cdr);
      AT(place).car = replace_ptr(AT(place).car, AT(c_car).cdr);
      ptr.hdr = NIL;
      DEBUG5("(%d,%d) replace_ptr(%d,%d,%d,)NIL"
             "-replace (1,2) with (9,10)\n",
             place.x, place.y,
             AT(place).cdr.hdr, AT(place).cdr.car, AT(place).cdr.cdr);
      AT(place).cdr = replace_ptr(AT(place).cdr, ptr);
    }
    /* 6. msg goes to 1 */
    msg.coord = place;
    enqueue(msg);
    /* 6. replace end of 10 with 4 */
    msg.mcar.hdr = EXTEND;
    msg.mcdr = copy_ptr(AT(c_cdr).cdr);
    enqueue(msg);
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
      case LAMBDA: if(! AT(c1).car.cdr) app_abs(place); break;
      case UNPACK:
        DEBUG8("(%d,%d) run->replace_ptr(%d,%d,%d,)(%d,%d,%d) c1\n",
               place.x, place.y,
               AT(place).car.hdr, AT(place).car.car, AT(place).car.cdr,
               AT(c1).cdr.hdr, AT(c1).cdr.car, AT(c1).cdr.cdr);
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
          DEBUG8("(%d,%d) run->replace_ptr(%d,%d,%d,)(%d,%d,%d) c2\n",
                 place.x, place.y,
                 AT(place).cdr.hdr, AT(place).cdr.car, AT(place).cdr.cdr,
                 AT(c2).cdr.hdr, AT(c2).cdr.car, AT(c2).cdr.cdr);
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
          DEBUG2("update_ref_msg(%d,%d) from curried function\n",
                 c2.x, c2.y);
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
    if(AT(place).car.hdr != LAMBDA ||
       AT(place).car.car != msg.mcar.car){
      /* lock the lambda msg when passing a lambda */
      if(AT(place).car.hdr == LAMBDA) msg.mcar.cdr = TRUE;
      DEBUG2("copy_ptr(%d,%d) from lambda message\n",
             msg.mcdr.car, msg.mcdr.cdr);
      copy_ptr(msg.mcdr);
      AT(place).cdr = lambda_app(msg, AT(place).cdr, AT(place).refs);
    }
    break;
  case EXTEND:
    switch(AT(place).cdr.hdr){
    case LOCAL:
      COORD_OF_PTR(c2, AT(place).cdr);
      msg.coord = c2;
      msg.mcar = AT(place).mcar;
      msg.mcdr = AT(place).mcdr;
      enqueue(msg);
      break;
    case NIL:
      DEBUG8("(%d,%d) run->replace_ptr(%d,%d,%d,)(%d,%d,%d) mcdr\n",
             place.x, place.y,
             AT(place).car.hdr, AT(place).car.car, AT(place).car.cdr,
             AT(place).mcdr.hdr, AT(place).mcdr.car, AT(place).mcdr.cdr);
      AT(place).cdr = replace_ptr(AT(place).cdr, AT(place).mcdr);
      delete_ptr(AT(place).mcdr);
      break;
    default:
      c1 = open_space(place);
      AT(c1).refs = 1;
      AT(c1).car = copy_ptr(AT(place).cdr);
      AT(c1).cdr = copy_ptr(AT(place).mcdr);
      delete_ptr(AT(place).mcdr);
      break;
    }
    break;
  }
  AT(place).mcar.hdr = NIL;
}
