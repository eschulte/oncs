/* Copyright (C) 2012 Eric Schulte */
#include "test.h"
#define EXPR_BUF_SIZE 1024

int verbose = 0;
int fail_p = 0;

void init(int argc, char *argv[]){
  int i;
  for(i=1;i<argc;i++)
    if(argv[i][0] == '-'){
      switch(argv[i][1]){
      case 'v': verbose = 1; break;
      case 'V': verbose = 2; break;
      }
    }
  debug(1, "");
}

int debug(int level, const char *format, ...){
  if(verbose >= level){
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
  }
}

int queue_population(){ return QWRAP(qend - qbeg); }

int population(){
  int i, j, count;
  count = 0;
  for(i=0;i<SIZE;i++)
    for(j=0;j<SIZE;j++)
      if (world[j][i].refs > 0)
        count++;
  return count;
}

int count(int type){
  int i, j, count;
  count = 0;
  for(i=0;i<SIZE;i++)
    for(j=0;j<SIZE;j++)
      if (world[j][i].refs > 0){
        if(world[j][i].car.hdr == type) count++;
        if(world[j][i].cdr.hdr == type) count++;
      }
  return count;
}

void show_ptr(ptr ptr){
  switch(ptr.hdr){
  case NIL:     printf("_"); break;
  case LOCAL:   printf("^"); break;
  case INTEGER: printf("i"); break;
  case SYMBOL:  printf("s"); break;
  case LAMBDA:  printf("l"); break;
  case PRIMOPT: printf("#"); break;
  case CURRIED: printf("@"); break;
  default:      printf("?"); break;
  }
}

void show_queue(){
  int i;
  msg msg;
  char c;
  printf("Q[%d]:", queue_population());
  for(i=0;i<(qend-qbeg);i++){
    msg = queue[(qbeg+i)];
    switch(msg.mcar.hdr){
    case NIL:     c='_'; break;
    case LOCAL:   c='^'; break;
    case INTEGER: c='i'; break;
    case SYMBOL:  c='s'; break;
    case LAMBDA:  c='l'; break;
    }
    printf("(%d,%d)%c", msg.coord.x, msg.coord.y, c);
  }
  printf("\n");
}

void show_world(){
  if(!verbose) return;
  onc tmp;
  int i, j;
  printf("\n");
  fflush(stdout);
  show_queue();
  for(i=-1;i<SIZE;i++) {
    for(j=-1;j<SIZE;j++) {
      /* index labels */
      if(i<0){
        if(j<0) printf("  ");
        else    printf(" %d ", j);
      } else {
        if(j<0) printf("%d ", i);
        else{
          tmp = world[j][i];
          if(tmp.refs > 0) {
            show_ptr(tmp.car);
            if(tmp.mcar.hdr == NIL) printf("%d", tmp.refs);
            else printf("I");
            show_ptr(tmp.cdr);
          } else printf("   ");
        }
      }
      if(j<(SIZE-1)) printf(" ");
    }
    printf("\n");
    fflush(stdout);
  }
}

int ptr_to_string(ptr ptr, char *buf, int index, int car_p){
  int i, j;
  char c;
  char s[20];
  coord coord;
  switch(ptr.hdr){
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
    }
    index++;
    break;
  case CURRIED:
    switch(ptr.car){
    case PLUS:   c = '+'; break;
    case MINUS:  c = '-'; break;
    case TIMES:  c = '*'; break;
    case DIVIDE: c = '/'; break;
    }
    i = sprintf(s, "%c%d", c, ptr.cdr);
    for(j=0;j<i;j++){
      buf[index] = s[j];
      index++;
    }
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

int string_to_onc(coord place, char *buf, int index){
  debug(2, "(%d,%d) %s:%d\n", place.x, place.y, buf, index);
  coord t1, t2;
  int i, parend;
  char *interum = buf;
  while(buf[index] != '\0') {
    AT(place).refs = 1;
    debug(2, "\tcar\n");
    STR_TO_PTR(AT(place).car, buf, index, t1);
    debug(2, "\tcdr\n");
    STR_TO_PTR(AT(place).cdr, buf, index, t1);
    place = open_space(place);
  }
}

int close_paren(char *buf, int index){
  int paren_counter;
  if(buf[index] = '(') paren_counter = 1;
  else                 paren_counter = 0;
  do {
    index++;
    switch(buf[index]){
    case '(': paren_counter++; break;
    case ')': paren_counter--; break;
    case '\0': printf("ERROR: unmatched paren %d\n", index); exit(1);
    default: break;
    }
  } while (paren_counter > 0);
  return index;
}

void show_all(coord place){
  char buf[EXPR_BUF_SIZE];
  show_world();
  onc_to_string(place, buf, 0);
  debug(1, "expr(%d,%d):%s\n", place.x, place.y, buf);
}

void run_down(coord place){
  coord tmp;
  run(place); show_all(place);
  while(queue_population() > 0){
    tmp = queue[qbeg].coord;
    debug(2, "run queue to (%d,%d)\n", tmp.x, tmp.y);
    run_queue(); show_all(place);
    debug(2, "run (%d,%d)\n", tmp.x, tmp.y);
    run(tmp);  show_all(place);
    debug(2, "checking queue\n");
  };
  debug(2, "leaving run_down\n");
}

void run_expr(char *expr, coord place){
  clear_world();
  string_to_onc(place, expr, 0);
  show_all(place);
  run_down(place);
  place.x = place.y = 4;
  run_down(place);
  debug(2, "leaving full_run\n");
}

int read_int(char *buf, int *index){
  int result, tmp;
  result = 0;
  while(0 <= (tmp = (buf[(*index)] - '0')) && tmp <= 9) {
    result = (result * 10) + tmp;
    (*index)++;
    tmp = (buf[(*index)] - '0');
  }
  return result;
}

void simple_app(coord place){
  coord tmp1, tmp2;
  /* setup world: ((lambda x (x x)) (1 2 3)) */
  AT(place).refs++;
  debug(2, "(%d,%d) -- ((lambda x (x x)) (1 2 3))\n", place.x, place.y);
  tmp1 = open_space(place);
  AT(tmp1).refs++;
  LOCAL_SET(place, car, tmp1);
  debug(2, "(%d,%d) -- (lambda x (x x))\n", tmp1.x, tmp1.y);
  tmp2 = open_space(place);
  AT(tmp2).refs++;
  LOCAL_SET(place, cdr, tmp2);
  debug(2, "(%d,%d) -- (1 2 3)\n", tmp2.x, tmp2.y);
  /* (lambda x (x x)) */
  place = tmp1;
  LAMBDA_SET(place, 1);
  tmp1 = open_space(place);
  AT(tmp1).refs++;
  debug(2, "(%d,%d) -- (x x)\n", tmp1.x, tmp1.y);
  LOCAL_SET(place, cdr, tmp1);
  /* (x x) */
  place = tmp1;
  SYMBOL_SET(place, car, 1);
  tmp1 = open_space(place);
  AT(tmp1).refs++;
  debug(2, "(%d,%d) -- (_ x)\n", tmp1.x, tmp1.y);
  LOCAL_SET(place, cdr, tmp1);
  SYMBOL_SET(tmp1, car, 1);
  NIL_SET(tmp1, cdr);
  /* (1 2 3) */
  place = tmp2;
  tmp2 = open_space(place);
  AT(tmp2).refs++;
  INTEGER_SET(place, car, 1);
  LOCAL_SET(place, cdr, tmp2);
  place = tmp2;
  tmp2 = open_space(place);
  AT(tmp2).refs++;
  debug(2, "(%d,%d) -- (_ 2 _)\n", place.x, place.y);
  INTEGER_SET(place, car, 2);
  LOCAL_SET(place, cdr, tmp2);
  place = tmp2;
  debug(2, "(%d,%d) -- (_ _ 3)\n", place.x, place.y);
  INTEGER_SET(place, car, 3);
  NIL_SET(place, cdr);
}
