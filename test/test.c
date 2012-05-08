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

void debug(int level, const char *format, ...){
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
  case LAMBDA: /* show lock status for lambda */
    if(ptr.cdr) printf("l");
    else        printf("L");
    break;
  case PRIMOPT: /* show lock status for primopt */
    if(ptr.cdr) printf("p");
    else        printf("P");
    break;
  /* show lock status for curried */
  case CURRIED: printf("C"); break;
  case LCURRIED: printf("c"); break;
  case BOOLEAN: printf("b"); break;
  case EXTEND:  printf("e"); break;
  default:      printf("?"); break;
  }
}

void show_queue(){
  msg msg;
  char c;
  int i, end;
  end = qend;
  if(!verbose) return;
  printf("Q[%d]:", queue_population());
  for(i=qbeg;i!=end;i=QWRAP(i+1)){
    msg = queue[i];
    switch(msg.mcar.hdr){
    case NIL:     c='_'; break;
    case LOCAL:   c='^'; break;
    case INTEGER: c='i'; break;
    case SYMBOL:  c='s'; break;
    case LAMBDA:  c='l'; break;
    case EXTEND:  c='e'; break;
    default:      c='?'; break;
    }
    printf("(%d,%d)%c", msg.place.x, msg.place.y, c);
  }
  printf("\n");
}

void show_world(){
  onc tmp;
  int i, j;
  if(!verbose) return;
  for(i=-1;i<SIZE;i++) {
    for(j=-1;j<SIZE;j++) {
      /* index labels */
      if(i<0){
        if(j<0) printf("  ");
        else    printf(" %2d", j);
      } else {
        if(j<0) printf("%2d ", i);
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

void string_to_onc(coord place, int locked, char *buf){
  char* bits;
  coord t1, c1, c2;
  char *interum;
  int i, car_p, parend, lock_child;
  unsigned int index;
  debug(2, "string_to_onc[%d]((%d,%d), %s)\n",
        locked, place.x, place.y, buf);
  index = 0;
  lock_child = FALSE;
  car_p = TRUE;
  interum = buf;
  while(buf[index] != '\0') {
    if(lock_child) locked = TRUE;
    while((buf[index] == '#') || (buf[index] == ' ')) index++;
    AT(place).refs = 1;
    if(car_p){
      if(buf[index] == '(') {
        /* push the sub-list to another call */
        t1 = open_space(place);
        AT(place).car.hdr = LOCAL;
        PTR_OF_COORD(AT(place).car, bits, t1);
        parend=close_paren(buf, index);
        index++;
        for(i=index;i<parend;i++) interum[(i-index)] = buf[i];
        interum[parend-index] = '\0';
        debug(2, "substring %s\n", interum);
        string_to_onc(t1, locked, interum);
        index = parend+1;
      } else {
        CHAR_TO_PTR(place, AT(place).car, buf[index], bits, c1, c2);
      }
      car_p = FALSE;
    } else {
      if(buf[index] == '.'){
        index++;
        while((buf[index] == '#') || (buf[index] == ' ')) index++;
        CHAR_TO_PTR(place, AT(place).cdr, buf[index], bits, c1, c2);
      } else {
        t1 = open_space(place);
        AT(place).cdr.hdr = LOCAL;
        PTR_OF_COORD(AT(place).cdr, bits, t1);
        place = t1;
      }
      car_p = TRUE;
    }
  }
  if(! car_p) AT(place).cdr.hdr = NIL;
}

int close_paren(char *buf, int index){
  int paren_counter;
  if(buf[index] == '(') paren_counter = 1;
  else                  paren_counter = 0;
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
  if(!verbose) return;
  printf("\n");
  fflush(stdout);
  show_queue();
  show_world();
  onc_to_string(place, buf, 0);
  debug(1, "expr(%d,%d):%s\n", place.x, place.y, buf);
}

void show_both(){
  if(!verbose) return;
  printf("\n");
  fflush(stdout);
  show_world();
  show_queue();
}

void run_down(coord place){
  coord tmp;
  debug(2, "run_down (%d, %d)\n", place.x, place.y);
  run(place); show_all(place);
  while(queue_population() > 0){
    tmp = queue[qbeg].place;
    debug(2, "run queue to (%d,%d)\n", tmp.x, tmp.y);
    run_queue(); show_all(place);
    debug(2, "run (%d,%d)\n", tmp.x, tmp.y);
    run(tmp);  show_all(place);
    debug(2, "checking queue\n");
  };
  debug(2, "leaving run_down\n");
}

unsigned long coord_hash(int x, int y){
  int i;
  unsigned long hash = 5381;
  int ints[12];
  ints[0]  = world[x][y].car.hdr;
  ints[1]  = world[x][y].car.car;
  ints[2]  = world[x][y].car.cdr;
  ints[3]  = world[x][y].cdr.hdr;
  ints[4]  = world[x][y].cdr.car;
  ints[5]  = world[x][y].cdr.cdr;
  ints[6]  = world[x][y].mcar.hdr;
  ints[7]  = world[x][y].mcar.car;
  ints[8]  = world[x][y].mcar.cdr;
  ints[9]  = world[x][y].mcdr.hdr;
  ints[10] = world[x][y].mcdr.car;
  ints[11] = world[x][y].mcdr.cdr;
  for(i=0;i<12;i++) hash = ((hash << 5) + hash) + ints[i];
  return hash;
}

unsigned long queue_hash(){
  int i, j;
  int msg_ints[8];
  unsigned long hash = 5381;
  i = qbeg;
  while(i != qend){
    msg_ints[0] = (int)queue[i].place.X;
    msg_ints[1] = (int)queue[i].place.x;
    msg_ints[2] = queue[i].mcar.hdr;
    msg_ints[3] = queue[i].mcar.car;
    msg_ints[4] = queue[i].mcar.cdr;
    msg_ints[5] = queue[i].mcdr.hdr;
    msg_ints[6] = queue[i].mcdr.car;
    msg_ints[7] = queue[i].mcdr.cdr;
    for(j=0;j<8;j++)
      hash = ((hash << 5) + hash) + msg_ints[j];
    i = QWRAP(i + 1);
  }
  return hash;
}

unsigned long world_hash(){
  int i, j;
  unsigned long hash = 0;
  for(i=0;i<SIZE;i++)
    for(j=0;j<SIZE;j++)
      hash = coord_hash(i, j) + (hash << 6) + (hash << 16) - hash;
  return hash;
}

int run_at(unsigned char x, unsigned char y){
  coord coord;
  debug(2, "running at (%d,%d)\n", x, y);
  coord.x = x; coord.y = y;
  return run(coord);
}

void run_one(){
  int i, j;
  for(i=0;i<SIZE;i++)
    for(j=0;j<SIZE;j++)
      if(world[i][j].refs > 0)
        if(run_at(i, j)) break;
}

void fix(coord place){
  msg msg;
  char buf[EXPR_BUF_SIZE];
  unsigned long hash_old, hash_new;
  hash_old = hash_new = 0;
  debug(2, "running to a fixed point\n");
  do{
    if(queue_population() > 0){
      msg = queue[qbeg];
      /* only pop from queue if dest. is empty */
      if(AT(msg.place).mcar.hdr == NIL){
        msg = dequeue();
        AT(msg.place).mcar = msg.mcar;
        AT(msg.place).mcdr = msg.mcdr;
      }
      /* run destination either way */
      run(msg.place);
    } else { run_one(); }
    printf("\n");
    show_world();
    onc_to_string(place, buf, 0);
    debug(1, "expr(%d,%d):%s\n", place.x, place.y, buf);
    hash_old = hash_new;
    hash_new = world_hash() ^ queue_hash();
    debug(1, "hash:%x\n", hash_new);
  } while( hash_old != hash_new );
}

void step(coord place){
  int i, j;
  char buf_strt[EXPR_BUF_SIZE];
  char buf_comp[EXPR_BUF_SIZE];
  unsigned long hash_old, hash_new;
  hash_old = hash_new = 0;
  onc_to_string(place, buf_strt, 0);
  do{
    if((queue_population() == 0) ||
       (! run_queue()))
      for(i=0;i<SIZE;i++)
        for(j=0;j<SIZE;j++){
          onc_to_string(place, buf_comp, 0);
          if(strcmp(buf_strt, buf_comp) != 0) break;
          if(world[i][j].refs > 0)
            if(run_at(i, j)) break;;
        }
    show_both();
    onc_to_string(place, buf_comp, 0);
    debug(1, "expr(%d,%d):%s\n",
          place.x, place.y, buf_comp);
    hash_old = hash_new;
    hash_new = world_hash() ^ queue_hash();
    debug(1, "hash:%x\n", hash_new);
  } while( hash_old != hash_new &&
           strcmp(buf_strt, buf_comp) == 0 );
}

void run_expr(char *expr, coord place){
  clear_world();
  string_to_onc(place, FALSE, expr);
  show_all(place);
  run_down(place);
  place.x = place.y = 4;
  run_down(place);
  debug(2, "leaving full_run\n");
}

int read_int(char *buf, unsigned int *index){
  int result, tmp;
  result = 0;
  while(buf[(*index)] &&
        0 <= (tmp = (buf[(*index)] - '0')) &&
        tmp <= 9) {
    result = (result * 10) + tmp;
    (*index)++;
    tmp = (buf[(*index)] - '0');
  }
  return result;
}

void expr_to_expr(char *expr){
  coord place;
  place.x = SIZE/2; place.y = SIZE/2;
  clear_world();
  string_to_onc(place, FALSE, expr);
  show_all(place);
  fix(place);
  onc_to_string(place, expr, 0);
}

void simple_app(coord place){
  char* bits;
  coord tmp1, tmp2;
  /* setup world: ((lambda x (x x)) (1 2 3)) */
  AT(place).refs++;
  debug(2, "(%d,%d) -- ((lambda x (x x)) (1 2 3))\n", place.x, place.y);
  tmp1 = open_space(place);
  AT(tmp1).refs++;
  LOCAL_SET(place, car, bits, tmp1);
  debug(2, "(%d,%d) -- (lambda x (x x))\n", tmp1.x, tmp1.y);
  tmp2 = open_space(place);
  AT(tmp2).refs++;
  LOCAL_SET(place, cdr, bits, tmp2);
  debug(2, "(%d,%d) -- (1 2 3)\n", tmp2.x, tmp2.y);
  /* (lambda x (x x)) */
  place = tmp1;
  LAMBDA_SET(place, 1);
  tmp1 = open_space(place);
  AT(tmp1).refs++;
  debug(2, "(%d,%d) -- (x x)\n", tmp1.x, tmp1.y);
  LOCAL_SET(place, cdr, bits, tmp1);
  /* (x x) */
  place = tmp1;
  SYMBOL_SET(place, car, 1);
  tmp1 = open_space(place);
  AT(tmp1).refs++;
  debug(2, "(%d,%d) -- (_ x)\n", tmp1.x, tmp1.y);
  LOCAL_SET(place, cdr, bits, tmp1);
  SYMBOL_SET(tmp1, car, 1);
  NIL_SET(tmp1, cdr);
  /* ((1 2 3)) */
  place = tmp2;
  tmp2 = open_space(place);
  AT(tmp2).refs++;
  LOCAL_SET(place, car, bits, tmp2);
  NIL_SET(place, cdr);
  /* (1 _ _) */
  place = tmp2;
  INTEGER_SET(place, car, 1);
  tmp2 = open_space(place);
  AT(tmp2).refs++;
  LOCAL_SET(place, cdr, bits, tmp2);
  /* (_ 2 _) */
  place = tmp2;
  INTEGER_SET(place, car, 2);
  tmp2 = open_space(place);
  AT(tmp2).refs++;
  LOCAL_SET(place, cdr, bits, tmp2);
  /* (_ _ 3) */
  place = tmp2;
  INTEGER_SET(place, car, 3);
  NIL_SET(place, cdr);
}
