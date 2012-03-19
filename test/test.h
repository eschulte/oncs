/* Copyright (C) 2012 Eric Schulte */
#include <string.h>
#include <stdarg.h>
#include <oncs.h>

#define SHOULD(x) if(!(x)) fail_p=1;

/* global state */
extern int verbose_p;
extern int fail_p;

/* setup */
void init(int argc, char *argv[]);

/* logging */
int debug(const char *format, ...);

/* world queries */
int queue_population();
int population();
int count(int type);
void show_ptr(ptr ptr);
void show_queue();
void show_world();
void get_expr(coord place, char *buf, int index);
int ptr_to_string(ptr ptr, char *buf, int index, int car_p);
int onc_to_string(coord place, char *buf, int index);
int string_to_onc(coord place, char *buf, int index);
int close_paren(char *buf, int index);

/* state placing macros */
#define NIL_SET(place, sub)              \
  AT(place).sub.hdr = NIL;
#define LOCAL_SET(palce, sub, coord)     \
  AT(place).sub.hdr = LOCAL;             \
  AT(place).sub.car = coord.x;           \
  AT(place).sub.cdr = coord.y;
#define INTEGER_SET(place, sub, int)     \
  AT(place).sub.hdr = INTEGER;           \
  AT(place).sub.car = int;
#define SYMBOL_SET(place, sub, variable) \
  AT(place).sub.hdr = SYMBOL;            \
  AT(place).sub.car = variable;
#define LAMBDA_SET(place, variable)      \
  AT(place).car.hdr = LAMBDA;            \
  AT(place).car.car = variable;
#define STR_TO_PTR(where, buf, index, t1)                       \
  while((buf[index] == '#') || (buf[index] == ' ')) index++;    \
  /* debug("\t%d:%c:%s\n", index, buf[index], buf); */          \
  switch(buf[index]){                                           \
  case ')': index++; /* NIL */                                  \
  case '\0':                                                    \
  /* debug("\tNIL:(%d,%d)\n", place.x, place.y); */             \
    where.hdr = NIL; break;                                     \
  case 'L': /* LAMBDA */                                        \
  /* debug("\tLAMBDA:(%d,%d)\n", place.x, place.y); */          \
    where.hdr = LAMBDA;                                         \
    index += 3;                                                 \
    where.car = read_int(buf, &index);                          \
    break;                                                      \
  case 'S': /* SYMBOL */                                        \
  /* debug("\tSYMBOL:(%d,%d)\n", place.x, place.y); */          \
    where.hdr = SYMBOL;                                         \
    index++;                                                    \
    where.car = read_int(buf, &index);                          \
    break;                                                      \
  case '(': /* LOCAL */                                         \
    t1 = open_space(place);                                     \
    /* debug("\tLOCAL:(%d,%d)\n", place.x, place.y); */         \
    where.hdr = LOCAL;                                          \
    where.car = t1.x; where.cdr = t1.y;                         \
    parend=close_paren(buf, index);                             \
    index++;                                                    \
    for(i=index;i<parend;i++) interum[(i-index)] = buf[i];      \
    interum[parend-index] = '\0';                               \
    string_to_onc(t1, interum, 0);                              \
    index = parend+1;                                           \
    break;                                                      \
  default: /* INTEGER */                                        \
    if(0 <= (buf[index] - '0') && (buf[index] - '0') <= 9) {    \
      /* debug("\tINTEGER:(%d,%d)\n", place.x, place.y); */     \
    where.hdr = INTEGER;                                        \
    where.car = read_int(buf, &index);                          \
    } else { index++; }                                         \
    break;                                                      \
  }

/* fixtures */
void simple_app(coord);
