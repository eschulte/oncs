/* Copyright (C) 2012 Eric Schulte */
#include <string.h>
#include <stdarg.h>
#include <oncs.h>

#define SHOULD(x) if(!(x)) fail_p=1;

/* global state */
extern int verbose;
extern int fail_p;

/* setup */
void init(int argc, char *argv[]);

/* logging */
void debug(int level, const char *format, ...);

/* world queries and actions */
int queue_population();
int population();
int count(int type);
void show_ptr(ptr ptr);
void show_queue();
void show_world();
void get_expr(coord place, char *buf, int index);
void string_to_onc(coord place, int locked, char *buf);
int close_paren(char *buf, int index);
void show_all(coord place);
void run_down(coord place);
void run_expr(char *expr, coord place);
int read_int(char *buf, unsigned int *index);
void expr_to_expr(char *expr);
void simple_app(coord place);

/* hashing functions from http://www.cse.yorku.ca/~oz/hash.html */
unsigned long coord_hash();
unsigned long world_hash();
int run_at(unsigned char x, unsigned char y);
void run_one();
void fix(coord place);
void step(coord place);

/* state placing macros */
#define NIL_SET(place, sub)              \
  AT(place).sub.hdr = NIL;
#define LOCAL_SET(palce, sub, coord)            \
  AT(place).sub.hdr = LOCAL;                    \
  PTR_OF_COORD(AT(place).sub, coord);
#define INTEGER_SET(place, sub, int)     \
  AT(place).sub.hdr = INTEGER;           \
  AT(place).sub.car = int;
#define SYMBOL_SET(place, sub, variable) \
  AT(place).sub.hdr = SYMBOL;            \
  AT(place).sub.car = variable;
#define LAMBDA_SET(place, variable)      \
  AT(place).car.hdr = LAMBDA;            \
  AT(place).car.car = variable;
#define CHAR_TO_PTR(place, where, char, c1, c2)                 \
  debug(2, "char_to_ptr((%d,%d), %c)\n",                        \
        place.x, place.y, char);                                \
  switch(char){                                                 \
  case '\0':                                                    \
    debug(2, "\tNIL:(%d,%d)\n", place.x, place.y);              \
    where.hdr = NIL; break;                                     \
  case 'l': /* LAMBDA */                                        \
  case 'L':                                                     \
    debug(2, "\tLAMBDA:(%d,%d)\n", place.x, place.y);           \
    where.hdr = LAMBDA;                                         \
    index++;                                                    \
    where.car = read_int(buf, &index);                          \
    where.cdr = locked;                                         \
    lock_child = TRUE;                                          \
    break;                                                      \
  case 'S': /* SYMBOL */                                        \
    debug(2, "\tSYMBOL:(%d,%d)\n", place.x, place.y);           \
    where.hdr = SYMBOL;                                         \
    index++;                                                    \
    where.car = read_int(buf, &index);                          \
    break;                                                      \
  case '+':                                                     \
    debug(2, "\tPLUS:(%d,%d)\n", place.x, place.y);             \
    where.hdr = PRIMOPT;                                        \
    where.car = PLUS;                                           \
    where.cdr = locked;                                         \
    index++;                                                    \
    break;                                                      \
  case '-':                                                     \
    debug(2, "\tMINUS:(%d,%d)\n", place.x, place.y);            \
    where.hdr = PRIMOPT;                                        \
    where.car = MINUS;                                          \
    where.cdr = locked;                                         \
    index++;                                                    \
    break;                                                      \
  case '*':                                                     \
    debug(2, "\tTIMES:(%d,%d)\n", place.x, place.y);            \
    where.hdr = PRIMOPT;                                        \
    where.car = TIMES;                                          \
    where.cdr = locked;                                         \
    index++;                                                    \
    break;                                                      \
  case '/':                                                     \
    debug(2, "\tDIVIDE:(%d,%d)\n", place.x, place.y);           \
    where.hdr = PRIMOPT;                                        \
    where.car = DIVIDE;                                         \
    where.cdr = locked;                                         \
    index++;                                                    \
    break;                                                      \
  case '=':                                                     \
    debug(2, "\tEQUAL:(%d,%d)\n", place.x, place.y);            \
    where.hdr = PRIMOPT;                                        \
    where.car = EQUAL;                                          \
    where.cdr = locked;                                         \
    index++;                                                    \
    break;                                                      \
  case '<':                                                     \
    debug(2, "\tLESS:(%d,%d)\n", place.x, place.y);             \
    where.hdr = PRIMOPT;                                        \
    where.car = LESS;                                           \
    where.cdr = locked;                                         \
    index++;                                                    \
    break;                                                      \
  case 't':                                                     \
    debug(2, "\tTRUE:(%d,%d)\n", place.x, place.y);             \
    BOOLEAN_APP(place, where, c1, c2, TRUE);                    \
    index++;                                                    \
    break;                                                      \
  case 'f':                                                     \
    debug(2, "\tFALSE:(%d,%d)\n", place.x, place.y);            \
    BOOLEAN_APP(place, where, c1, c2, FALSE);                   \
    index++;                                                    \
    break;                                                      \
  default: /* INTEGER */                                        \
    if(0 <= (buf[index] - '0') && (buf[index] - '0') <= 9) {    \
      debug(2, "\tINTEGER:(%d,%d)\n", place.x, place.y);        \
      where.hdr = INTEGER;                                      \
      where.car = read_int(buf, &index);                        \
    } else { index++; }                                         \
    break;                                                      \
  }

/* fixtures */
void simple_app(coord);
