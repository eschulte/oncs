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
  AT(msg.place).num_msgs++;
}

msg dequeue(){
  msg msg;
  if(queue[qbeg].mcar.hdr != NIL){
    qbeg = QWRAP(qbeg+1);
    msg = queue[QWRAP(qbeg-1)];
    AT(msg.place).num_msgs--;
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

int free_p(coord place){
  return (AT(place).refs == 0 &&
          ! has_incoming_msgs(place));
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
    if (free_p(place)) break;
  } while (tried <= SIZE*SIZE);
  if (tried >= SIZE*SIZE) { ERROR("exhausted world"); }
  else { clear_place(place); return place; }
}

int run_queue(){
  if(queue[qbeg].mcar.hdr != NIL){
    msg msg = dequeue();
    if(AT(msg.place).mcar.hdr == NIL){
      AT(msg.place).mcar = msg.mcar;
      AT(msg.place).mcdr = msg.mcdr;
      return 1;
    } else {
      run(msg.place);
      DEBUG2("message requeue: (%d,%d)\n",
             msg.place.x, msg.place.y);
      enqueue(msg);
      return 0;
    }
  }
  return 0;
}

int has_incoming_msgs(coord place){
  coord temp;
  if(AT(place).num_msgs){
    return TRUE;
  } else {
    if(AT(place).car.hdr == LOCAL){
      COORD_OF_PTR(temp, AT(place).car);
      if(has_incoming_msgs(temp))
        return TRUE;
    }
    if(AT(place).cdr.hdr == LOCAL){
      COORD_OF_PTR(temp, AT(place).cdr);
      if(has_incoming_msgs(temp))
        return TRUE;
    }
  }
  return FALSE;
}

void update_ref_msg(coord place, int diff){
  msg msg;
  msg.place = place;
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

ptr replace_ptr(ptr old_p, ptr new_p){
  new_p = copy_ptr(new_p);
  delete_ptr(old_p);
  return new_p;
}

int argument_p(ptr ptr){
  #if EVALUATION_STRATEGY == CALL_BY_NAME
  return !(ptr.hdr == NIL);
  #elif EVALUATION_STRATEGY == CALL_BY_VALUE
  return value_p(ptr);
  #endif
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
      return ! argument_p(AT(place).cdr);
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
      /* guess we don't need to recurse here */
      return TRUE;
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
  case NIL:
    if(car_p) { buf[index] = '_'; index++; }
    else      { index--; }
    break;
  case LOCAL:
    COORD_OF_PTR(coord, ptr);
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

ptr lambda_app(msg l_msg, coord place, int dir){
  ptr ptr;
  msg msg;
  if(dir == LEFT) ptr = AT(place).car;
  else            ptr = AT(place).cdr;
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
    if(l_msg.mcar.car == ptr.car){
      if(l_msg.mcdr.hdr == LOCAL){
        /* make an empty LOCAL pointer */
        msg.place = open_space(place);
        AT(msg.place).refs = AT(place).refs;
        ptr.hdr = LOCAL;
        PTR_OF_COORD(ptr, msg.place);
        if(l_msg.mcar.cdr) msg.mcar.hdr = LDUPLICATE;
        else               msg.mcar.hdr = DUPLICATE;
        PTR_OF_COORD(msg.mcar, msg.place);
        COORD_OF_PTR(msg.place, l_msg.mcdr);
        msg.mcdr.hdr=0;
        msg.mcdr.car=0;
        msg.mcdr.cdr=0;
        APPEND(msg.mcdr.cdr, dir, msg.mcdr.hdr);
        msg.mcdr.hdr--;
        enqueue(msg);
      } else {
        ptr = l_msg.mcdr;
      }
    }
    break;
  case LOCAL:
    COORD_OF_PTR(l_msg.place, ptr);
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
  msg msg;
  ptr ptr;
  int ran;
  coord c_car, c_cdr, c_tmp;
  ran = FALSE;
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
  DEBUG3("has_incoming_msgs(%d,%d) == %d\n",
         place.x, place.y, has_incoming_msgs(place));
  if(/* don't apply to non values -- call-by-value */
     argument_p(AT(place).cdr) &&
     /* only apply when body has 0 incoming λ-messages */
     has_incoming_msgs(place) == 0){
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
    msg.place = place;
    enqueue(msg);
    /* 6. replace end of 10 with 4 */
    if(AT(c_cdr).cdr.hdr != NIL){
      msg.mcar.hdr = EXTEND;
      msg.mcdr = copy_ptr(AT(c_cdr).cdr);
      enqueue(msg);
    }
    ran = TRUE;
  }
  return ran;
}

int run(coord place){
  msg msg;
  coord c1, c2;
  int ran, i1, i2, locked;
  locked = ran = FALSE;
  switch(AT(place).mcar.hdr){
  case NIL:                     /* waiting loop */
    switch(AT(place).car.hdr){
    case LOCAL:
      COORD_OF_PTR(c1, AT(place).car);
      /* apply unlocked λ's */
      if(AT(c1).car.hdr == LAMBDA &&
         AT(c1).car.cdr == FALSE)
        ran = app_abs(place);
      /* eliminate redundant nested parenthesis */
      else if(AT(c1).cdr.hdr == NIL &&
              has_incoming_msgs(c1) == 0){
        AT(place).car = replace_ptr(AT(place).car, AT(c1).car);
        ran = TRUE;
      }
      break;
    case PRIMOPT:
      if(AT(place).car.cdr == FALSE){ /* locked? */
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
            DEBUG8("(%d,%d) run->rep_ptr(%d,%d,%d,)(%d,%d,%d) c2\n",
                   place.x, place.y,
                   AT(place).cdr.hdr,
                   AT(place).cdr.car, AT(place).cdr.cdr,
                   AT(c2).cdr.hdr, AT(c2).cdr.car, AT(c2).cdr.cdr);
            AT(place).cdr = replace_ptr(AT(place).cdr, AT(c2).cdr);
          }
          break;
        default: ran = FALSE;
        }
        break;
      }
      break;
    case LCURRIED: break; /* don't touch if locked */
    case CURRIED:
      if(AT(place).cdr.hdr != LOCAL)
        ERROR("curried application must point to something");
      COORD_OF_PTR(c2, AT(place).cdr);
      if(/* don't apply to non values -- call-by-value */
         argument_p(AT(place).cdr) &&
         /* only apply when body has 0 incoming λ-messages */
         has_incoming_msgs(place) == 0 &&
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
    if(AT(place).refs == 0 && AT(place).num_msgs != 0){
      AT(place).refs -= AT(place).mcar.car;
      msg.mcar = AT(place).mcar;
      msg.place = place;
      enqueue(msg);
    } else {
      msg.mcar = AT(place).mcar;
      INTEGER_APP(AT(place).car, msg);
      INTEGER_APP(AT(place).cdr, msg);
    }
    ran = TRUE;
    break;
  case LAMBDA:                  /* perform lambda application */
    msg.mcar = AT(place).mcar; msg.mcdr = AT(place).mcdr;
    if(AT(place).mcdr.hdr == LOCAL) COORD_OF_PTR(c2, AT(place).mcdr);
    AT(place).car = lambda_app(msg, place, LEFT);
    /* don't descend down shadowing lambdas */
    if(AT(place).car.hdr != LAMBDA ||
       AT(place).car.car != msg.mcar.car){
      /* lock the lambda msg when passing a lambda */
      if(AT(place).car.hdr == LAMBDA) msg.mcar.cdr = TRUE;
      DEBUG2("copy_ptr(%d,%d) from lambda message\n",
             msg.mcdr.car, msg.mcdr.cdr);
      copy_ptr(msg.mcdr);
      AT(place).cdr = lambda_app(msg, place, RIGHT);
    }
    ran = TRUE;
    break;
  case EXTEND:
    switch(AT(place).cdr.hdr){
    case LOCAL:
      COORD_OF_PTR(c2, AT(place).cdr);
      msg.place = c2;
      msg.mcar = AT(place).mcar;
      msg.mcdr = AT(place).mcdr;
      enqueue(msg);
      break;
    case NIL:
      DEBUG8("(%d,%d) run->replace_ptr(%d,%d,%d,)(%d,%d,%d) mcdr\n",
             place.x, place.y,
             AT(place).car.hdr, AT(place).car.car, AT(place).car.cdr,
             AT(place).mcdr.hdr,
             AT(place).mcdr.car, AT(place).mcdr.cdr);
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
  case LDUPLICATE:
    locked = TRUE;
  case DUPLICATE:
    msg.mcar.hdr = REPLACE;
    COORD_OF_PTR(msg.place, AT(place).mcar);
    /* send off car message */
    msg.mcar = AT(place).mcdr;
    APPEND(msg.mcar.cdr, LEFT, msg.mcar.hdr);
    if(locked) msg.mcar.hdr = LREPLACE;
    else       msg.mcar.hdr = REPLACE;
    msg.mcdr = AT(place).car;
    enqueue(msg);
    /* send off cdr message */
    msg.mcar = AT(place).mcdr;
    /* the bodies of λs should be locked after insertion */
    if(AT(place).car.hdr == LAMBDA) locked=TRUE;
    APPEND(msg.mcar.cdr, RIGHT, msg.mcar.hdr);
    if(locked) msg.mcar.hdr = LREPLACE;
    else       msg.mcar.hdr = REPLACE;
    msg.mcdr = AT(place).cdr;
    enqueue(msg);
    /* propagate downwards car */
    if(AT(place).car.hdr == LOCAL){
      msg.mcar = AT(place).mcar;
      COORD_OF_PTR(msg.place, AT(place).car);
      /* add a LEFT to msg.mcdr */
      msg.mcdr = AT(place).mcdr;
      msg.mcdr.car++;
      if(AT(place).cdr.hdr == LOCAL)
        APPEND(msg.mcdr.cdr, LEFT, msg.mcdr.hdr);
      enqueue(msg);
    }
    /* propagate downwards cdr */
    if(AT(place).cdr.hdr == LOCAL){
      msg.mcar = AT(place).mcar;
      /* the bodies of λs should be locked after insertion */
      if(AT(place).car.hdr == LAMBDA)
        msg.mcar.hdr = LDUPLICATE;
      COORD_OF_PTR(msg.place, AT(place).cdr);
      /* add a RIGHT to msg.mcdr */
      msg.mcdr = AT(place).mcdr;
      msg.mcdr.car++;
      if(AT(place).car.hdr == LOCAL)
        APPEND(msg.mcdr.cdr, RIGHT, msg.mcdr.hdr);
      enqueue(msg);
    }
    ran = TRUE;
    break;
  case LREPLACE:
    locked = TRUE;
  case REPLACE:
    if(AT(place).mcar.car > 0){
      /* propagate down the queue */
      if(AT(place).car.hdr == LOCAL &&
         AT(place).cdr.hdr == LOCAL){
        /* propagate down the ONC */
        POP(AT(place).mcar.cdr, i1);
        if(i1 == LEFT){
          COORD_OF_PTR(msg.place, AT(place).car);
        } else {
          COORD_OF_PTR(msg.place, AT(place).cdr);
        }
      } else if(AT(place).car.hdr == LOCAL){
        COORD_OF_PTR(msg.place, AT(place).car);
      } else if(AT(place).cdr.hdr == LOCAL){
        COORD_OF_PTR(msg.place, AT(place).cdr);
      } else {
        printf("(%d,%d) depth(%d) value=(%d,%d,%d)\n",
               place.x, place.y, AT(place).mcar.car,
               AT(place).mcdr.hdr,
               AT(place).mcdr.car,
               AT(place).mcdr.cdr);
        ERROR("can't propagate down bottomed out ONC.");
      }
      msg.mcar = AT(place).mcar;
      msg.mcar.car = AT(place).mcar.car-1;
      msg.mcdr = AT(place).mcdr;
      enqueue(msg);
    } else {
      /* apply here */
      POP(AT(place).mcar.cdr, i1);
      if(AT(place).mcdr.hdr == LOCAL){
        c1 = open_space(place);
        AT(c1).refs = AT(place).refs;
        if(i1 == LEFT){
          AT(place).car.hdr = LOCAL;
          PTR_OF_COORD(AT(place).car, c1);
        } else {
          AT(place).cdr.hdr = LOCAL;
          PTR_OF_COORD(AT(place).cdr, c1);
        }
      } else {
        if(i1 == LEFT){
          AT(place).car = AT(place).mcdr;
        } else {
          AT(place).cdr = AT(place).mcdr;
        }
        switch(AT(place).car.hdr){
        case LCURRIED:
        case CURRIED:
          if(i1 == LEFT){
            if(locked) AT(place).car.hdr = LCURRIED;
            else       AT(place).car.hdr = CURRIED;
          } else {
            if(locked) AT(place).cdr.hdr = LCURRIED;
            else       AT(place).cdr.hdr = CURRIED;
          }
          break;
        case PRIMOPT:
        case LAMBDA:
          if(i1 == LEFT) AT(place).car.cdr = locked;
          else           AT(place).cdr.cdr = locked;
          break;
        }
      }
    }
    ran = TRUE;
    break;
  }
  if(ran) AT(place).mcar.hdr = NIL;
  return ran;
}
