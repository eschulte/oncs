/* Copyright (C) 2012 Eric Schulte */
#include "test.h"

int verbose_p = 0;
int fail_p = 0;

void init(int argc, char *argv[]){
  int i;
  for(i=1;i<argc;i++)
    if(strcmp(argv[i], "-v") == 0)
      verbose_p=1;
}

int debug(const char *format, ...){
    va_list args;
    va_start(args, format);
    if(verbose_p) vprintf(format, args);
    va_end(args);
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

void show_ptr(ptr ptr){
  switch(ptr.hdr){
  case NIL:     printf("_"); break;
  case LOCAL:   printf("^"); break;
  case INTEGER: printf("i"); break;
  case SYMBOL:  printf("s"); break;
  case LAMBDA:  printf("l"); break;
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
  if(!verbose_p) return;
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

void simple_app(coord place){
  coord tmp1, tmp2;
  int hold = verbose_p;
  verbose_p = 0;
  /* setup world: ((lambda x (x x)) (1 2 3)) */
  AT(place).refs++;
  debug("(%d,%d) -- ((lambda x (x x)) (1 2 3))\n", place.x, place.y);
  tmp1 = open_space(place);
  AT(tmp1).refs++;
  LOCAL_SET(place, car, tmp1);
  debug("(%d,%d) -- (lambda x (x x))\n", tmp1.x, tmp1.y);
  tmp2 = open_space(place);
  AT(tmp2).refs++;
  LOCAL_SET(place, cdr, tmp2);
  debug("(%d,%d) -- (1 2 3)\n", tmp2.x, tmp2.y);
  /* (lambda x (x x)) */
  place = tmp1;
  LAMBDA_SET(place, 1);
  tmp1 = open_space(place);
  AT(tmp1).refs++;
  debug("(%d,%d) -- (x x)\n", tmp1.x, tmp1.y);
  LOCAL_SET(place, cdr, tmp1);
  /* (x x) */
  place = tmp1;
  SYMBOL_SET(place, car, 1);
  tmp1 = open_space(place);
  AT(tmp1).refs++;
  debug("(%d,%d) -- (_ x)\n", tmp1.x, tmp1.y);
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
  debug("(%d,%d) -- (_ 2 _)\n", place.x, place.y);
  INTEGER_SET(place, car, 2);
  LOCAL_SET(place, cdr, tmp2);
  place = tmp2;
  debug("(%d,%d) -- (_ _ 3)\n", place.x, place.y);
  INTEGER_SET(place, car, 3);
  NIL_SET(place, cdr);
  verbose_p = hold;
}
