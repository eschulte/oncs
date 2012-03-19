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

/* fixtures */
void simple_app(coord);
