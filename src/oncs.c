/* Copyright (C) 2012 Eric Schulte */
#include <stdlib.h>
#include <oncs.h>

onc world[SIZE][SIZE];
msg queue[QLENGTH];
int qbeg, qend = 0;

void enqueue(msg msg){
  queue[qend]=msg;
  qend = QWRAP(qend+1);
  if(qend == qbeg) ERROR("queue overflow");
  if(msg.mcar.hdr == LAMBDA) AT(msg.coord).l_msgs++;
}

msg dequeue(){
  msg msg;
  if(queue[qbeg].mcar.hdr != NIL){
    qbeg = QWRAP(qbeg+1);
    msg = queue[QWRAP(qbeg-1)];
    if(msg.mcar.hdr == LAMBDA) AT(msg.coord).l_msgs--;
    return msg;
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

int has_incoming_lambda(coord place){
  coord temp;
  if(AT(place).l_msgs){
    return TRUE;
  } else {
    if(AT(place).car.hdr == LOCAL){
      COORD_OF_PTR(temp, AT(place).car);
      if(has_incoming_lambda(temp))
        return TRUE;
    }
    if(AT(place).cdr.hdr == LOCAL){
      COORD_OF_PTR(temp, AT(place).cdr);
      if(has_incoming_lambda(temp))
        return TRUE;
    }
  }
  return FALSE;
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

void duplicate_msgs(coord from, coord to){
  int i, end;
  end = qend;
  msg msg;
  for(i=qbeg;i<end;i=QWRAP(i+1)){
    msg = queue[i];
    if(msg.mcar.hdr != INTEGER &&
       msg.coord.x == from.x &&
       msg.coord.y == from.y){
      msg.coord = to;
      enqueue(msg);
    }
  }
}

ptr duplicate_ptr(ptr old_p, int refs, int locked){
  coord orig, new;
  ptr new_p;
  new_p = old_p;
  if(new_p.hdr == LOCAL){
    COORD_OF_PTR(orig, new_p);
    new = open_space(orig);
    AT(new).refs = refs;
    new_p.car = new.x; new_p.cdr = new.y;
    duplicate_msgs(orig, new);
    AT(new).car = duplicate_ptr(AT(orig).car, refs, locked);
    /* the bodies of lambdas should be locked after insertion */
    if(AT(orig).car.hdr == LAMBDA) locked = TRUE;
    AT(new).cdr = duplicate_ptr(AT(orig).cdr, refs, locked);
  } else {
    switch(new_p.hdr){
    case LCURRIED:
    case CURRIED:
      if(locked) new_p.hdr = LCURRIED;
      else       new_p.hdr = CURRIED;
      return new_p;
      break;
    case LAMBDA:
    case PRIMOPT:
      new_p.cdr = locked;
    default:
      break;
    }
  }
  return new_p;
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
    /* if(ptr.cdr) buf[index] = '_'; index++; */
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
  case LCURRIED:
    /* buf[index] = '_'; index++; */
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
  /* lock lambda primopt or curried if message is locked */
  case PRIMOPT:
    ptr.cdr = l_msg.mcar.cdr; break;
  case CURRIED:
  case LCURRIED:
    if (l_msg.mcar.cdr) ptr.hdr = LCURRIED;
    else                ptr.hdr = CURRIED;
    break;
  case LAMBDA:
    DEBUG3("L%d locking %d->%d\n",
           ptr.car, ptr.cdr, l_msg.mcar.cdr);
    ptr.cdr = l_msg.mcar.cdr;
    break;
  case SYMBOL:
    if(l_msg.mcar.car == ptr.car)
      ptr = duplicate_ptr(l_msg.mcdr, refs, l_msg.mcar.cdr);
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

int app_abs(coord place){
  int ran;
  ran = FALSE;
  ptr ptr;
  msg msg;
  coord c_car, c_cdr, c_tmp;
  /* setup */
  if(AT(place).car.hdr != LOCAL)
    ERROR("malformed app_abs");
  COORD_OF_PTR(c_car, AT(place).car);
  DEBUG4("app_abs(%d,%d)<-body-(%d,%d)\n",
         place.x, place.y, c_car.x, c_car.y);
  if(AT(c_car).car.hdr != LAMBDA ||
     (AT(c_car).cdr.hdr != LOCAL &&
      AT(c_car).cdr.hdr != NIL))
    ERROR("malformed app_abs.car");
  if(AT(place).cdr.hdr == LOCAL)
    COORD_OF_PTR(c_cdr, AT(place).cdr);
  DEBUG3("has_incoming_lambda(%d,%d) == %d\n",
         place.x, place.y, has_incoming_lambda(place));
  if(/* don't apply to non values -- call-by-value */
     value_p(AT(place).cdr) &&
     /* only apply when body has 0 incoming λ-messages */
     has_incoming_lambda(place) == 0){
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
      COORD_OF_PTR(c_tmp, AT(c_car).cdr);
      DEBUG8("(%d,%d) replace_ptr(%d,%d,%d,)(%d,%d,%d)"
             "-replace (1,2) with (9,10)\n",
             place.x, place.y,
             AT(place).car.hdr, AT(place).car.car, AT(place).car.cdr,
             AT(c_tmp).car.hdr, AT(c_tmp).car.car, AT(c_tmp).car.cdr);
      AT(place).car = replace_ptr(AT(place).car, AT(c_tmp).car);
      DEBUG8("(%d,%d) replace_ptr(%d,%d,%d,)(%d,%d,%d)"
             "-replace (1,2) with (9,10)\n",
             place.x, place.y,
             AT(place).cdr.hdr, AT(place).cdr.car, AT(place).cdr.cdr,
             AT(c_tmp).cdr.hdr, AT(c_tmp).cdr.car, AT(c_tmp).cdr.cdr);
      AT(place).cdr = replace_ptr(AT(place).cdr, AT(c_tmp).cdr);
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
    ran = TRUE;
  }
  return ran;
}

int run(coord place){
  int ran;
  ran = FALSE;
  msg msg;
  coord c1, c2;
  int i1, i2;
  switch(AT(place).mcar.hdr){
  case NIL:                     /* waiting loop */
    switch(AT(place).car.hdr){
    case LOCAL:
      COORD_OF_PTR(c1, AT(place).car);
      /* apply unlocked λ's */
      if(AT(c1).car.hdr == LAMBDA && AT(c1).car.cdr == FALSE)
        ran = app_abs(place);
      /* eliminate redundant nested parenthesis */
      else if(AT(c1).cdr.hdr == NIL && has_incoming_lambda(c1) == 0){
        AT(place).car = replace_ptr(AT(place).car, AT(c1).car);
        ran = TRUE;
      }
      break;
    case PRIMOPT:
      if(AT(place).car.cdr == FALSE){ /* only PRIMOPT if unlocked */
        ran = TRUE;
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
          }
          break;
        default: ran = FALSE;
        }
        break;
      }
      break;
    case LCURRIED: break; /* don't act upon locked curried messages */
    case CURRIED:
      if(AT(place).cdr.hdr != LOCAL)
        ERROR("curried application must point to something");
      COORD_OF_PTR(c2, AT(place).cdr);
      if(/* don't apply to non values -- call-by-value */
         value_p(AT(place).cdr) &&
         /* only apply to integers */
         AT(c2).car.hdr == INTEGER){
        i1 = AT(place).car.cdr; i2 = AT(c2).car.car;
        switch(AT(place).car.car){
        case PLUS:   i1 = i2 + i1; break;
        case MINUS:  i1 = i2 - i1; break;
        case TIMES:  i1 = i2 * i1; break;
        case DIVIDE: i1 = i2 / i1; break;
        case EQUAL: if(i2 == i1) i1 = TRUE; else i1 = FALSE; break;
        case LESS:  if(i2  < i1) i1 = TRUE; else i1 = FALSE; break;
        default: ERROR("unsupported CURRIED AT(place).car"); break;
        }
        if(AT(place).car.car <= DIVIDE) AT(place).car.hdr = INTEGER;
        else                            AT(place).car.hdr = BOOLEAN;
        AT(place).car.car = i1;
        AT(place).cdr = copy_ptr(AT(c2).cdr);
        DEBUG2("update_ref_msg(%d,%d) from curried function\n",
               c2.x, c2.y);
        update_ref_msg(c2, -1);
        ran = TRUE;
      }
      break;
    }
    if(AT(place).car.hdr == BOOLEAN){
      BOOLEAN_APP(place, AT(place).car, c1, c2, i1);
      ran = TRUE;
    }
    if(AT(place).cdr.hdr == BOOLEAN){
      BOOLEAN_APP(place, AT(place).cdr, c1, c2, i2);
      ran = TRUE;
    }
    break;
  case INTEGER:                 /* update number of references */
    AT(place).refs += AT(place).mcar.car;
    msg.mcar = AT(place).mcar;
    INTEGER_APP(AT(place).car, msg);
    INTEGER_APP(AT(place).cdr, msg);
    ran = TRUE;
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
    ran = TRUE;
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
    ran = TRUE;
    break;
  }
  /* TODO: don't nullify message if we haven't ran? */
  if(ran) AT(place).mcar.hdr = NIL;
  return ran;
}
