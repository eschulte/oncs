/* Copyright (C) 2012 Eric Schulte */
#include <stdlib.h>
#include <stdio.h>

/* types of pointers */
#define NIL     0
#define LOCAL   1
#define INTEGER 2
#define SYMBOL  3
#define LAMBDA  4
#define PRIMOPT 5
#define CURRIED 6
#define UNPACK  7
#define BOOLEAN 8

/* types of primitive operations */
#define PLUS   0
#define MINUS  1
#define TIMES  2
#define DIVIDE 3
#define EQUAL  4
#define LESS   5
#define TRUE   1
#define FALSE  0

/* world size and access */
#define SIZE 10
#define QLENGTH 1000
#define AT(point) world[point.x][point.y]
#define WRAP(x) (SIZE+x)%SIZE
#define QWRAP(x) x%QLENGTH
#define COORD_OF_PTR(coord, ptr) \
  { coord.x=ptr.car; coord.y=ptr.cdr; }
#define INTEGER_APP(where, msg)                 \
  if(where.hdr == LOCAL){                       \
    COORD_OF_PTR(msg.coord, where);             \
    DEBUG("enqueue from INTEGER_APP\n");        \
    enqueue(msg);                               \
  }
#define CURRIED_APP(op, arg) {                                  \
    switch(op.car.car){                                         \
    case PLUS:   op.cdr.car = op.car.cdr + arg.car; break;      \
    case MINUS:  op.cdr.car = op.car.cdr - arg.car; break;      \
    case TIMES:  op.cdr.car = op.car.cdr * arg.car; break;      \
    case DIVIDE: op.cdr.car = op.car.cdr / arg.car; break;      \
    case EQUAL:                                                 \
      if(op.car.cdr == arg.car) op.cdr.car = TRUE;              \
      else op.cdr.car = FALSE;                                  \
      break;                                                    \
    case LESS:                                                  \
      if(op.car.cdr < arg.car) op.cdr.car = TRUE;               \
      else op.cdr.car = FALSE;                                  \
      break;                                                    \
    default: ERROR("unsupported CURRIED op.careration"); break; \
    }                                                           \
    if(op.car.car <= DIVIDE) op.cdr.hdr = INTEGER;              \
    else                     op.cdr.hdr = BOOLEAN;              \
    op.car.hdr  = UNPACK;                                       \
  }
#define BOOLEAN_APP(place, ptr, c1, c2, i1) {           \
    c1 = open_space(place);                             \
    AT(c1).refs = 1;                                    \
    i1 = ptr.car;                                       \
    ptr.hdr = LOCAL;                                    \
    ptr.car = c1.x;                                     \
    ptr.cdr = c1.y;                                     \
    c2 = open_space(c1);                                \
    AT(c2).refs = 1;                                    \
    AT(c1).car.hdr = LAMBDA;                            \
    AT(c1).car.car = TRUE;                              \
    AT(c1).cdr.hdr = LOCAL;                             \
    AT(c1).cdr.car = c2.x;                              \
    AT(c1).cdr.cdr = c2.y;                              \
    c1 = open_space(c2);                                \
    AT(c1).refs = 1;                                    \
    AT(c2).car.hdr = LAMBDA;                            \
    AT(c2).car.car = FALSE;                             \
    AT(c2).cdr.hdr = LOCAL;                             \
    AT(c2).cdr.car = c1.x;                              \
    AT(c2).cdr.cdr = c1.y;                              \
    AT(c1).car.hdr = SYMBOL;                            \
    AT(c1).car.car = i1;                                \
    AT(c1).cdr.hdr = NIL;                               \
  }

/* utility macros */
#define DEBUG_P 0
#define DEBUG(fmt) if(DEBUG_P) printf(fmt);
#define DEBUG1(fmt, x) if(DEBUG_P) printf(fmt, x);
#define DEBUG2(fmt, x, y) if(DEBUG_P) printf(fmt, x, y);
#define DEBUG3(fmt, x, y, z) if(DEBUG_P) printf(fmt, x, y, z);
#define ERROR(message) { printf("ERROR: %s\n", message); exit(1); }

/* structures inhabiting the world */
typedef struct { int x, y; } coord;
typedef struct { int hdr, car, cdr; } ptr;
typedef struct { ptr mcar, mcdr; coord coord; } msg;
typedef struct { ptr car, cdr, mcar, mcdr; int refs; } onc;

/* global state */
extern onc world[SIZE][SIZE];
extern msg queue[QLENGTH];
extern int qbeg, qend;

/* message queue operations */
void enqueue(msg msg);
msg dequeue();

/* set all ref counters to 0 clearing out the world */
void clear_world();

/* forcefully empty the queue */
void clear_queue();

/* return the coord of the nearest open space
 * 
 * cycle around original place
 * ===========================
 *        ^       1
 *        |    ^ --> |
 *       3|   1|     |2
 *        | <------- v
 *               2
 */
coord open_space(coord place);

/* try to apply a message from the queue */
int run_queue();

/* send a message to PLACE to change its ref counter by DIFF */
void update_ref_msg(coord place, int diff);

/* pointer maintenance */
ptr copy_ptr(ptr ptr);
ptr delete_ptr(ptr ptr);
ptr replace_ptr(ptr old, ptr new);
ptr duplicate_ptr(ptr ptr, int refs);
int value_p(ptr ptr);

/* read strings out from onc world */
int ptr_to_string(ptr ptr, char *buf, int index, int car_p);
int onc_to_string(coord place, char *buf, int index);

/* apply the lambda message L_MSG to PTR */
ptr lambda_app(msg l_msg, ptr ptr, int refs);

/* perform lambda application at PLACE */
void app_abs(coord place);

/* run the onc at PLACE according to its contents and message */
void run(coord place);
