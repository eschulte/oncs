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
#define DUPLICATE_LOCAL(place, near, tmp_from, tmp_to) {        \
    tmp_from.x = place.car;                                     \
    tmp_from.y = place.cdr;                                     \
    tmp_to = open_space(near);                                  \
    duplicate(tmp_to, tmp_from);                                \
  }
#define COORD_OF_PTR(coord, ptr) \
  { coord.x=ptr.car; coord.y=ptr.cdr; }
#define INTEGER_APP(where, msg)                 \
  if(where.hdr == LOCAL){                       \
    COORD_OF_PTR(msg.coord, AT(place).car);     \
    DEBUG("enqueue from INTEGER_APP\n");        \
    enqueue(msg);                               \
  }
#define LAMBDA_APP(l_msg, where, c1)                    \
  switch(where.hdr){                                    \
  case NIL:                                             \
  case INTEGER: break;                                  \
  case BOOLEAN: break;                                  \
  case SYMBOL:                                          \
    if(l_msg.mcar.car == where.car)                     \
      where = copy_ptr(l_msg.mcdr);                     \
    break;                                              \
  case LAMBDA:                                          \
    if(l_msg.mcar.car == where.car) return;             \
    break;                                              \
  case LOCAL:                                           \
    COORD_OF_PTR(l_msg.coord, where);                   \
    if(l_msg.mcdr.hdr == LOCAL) {                       \
      c1.x = l_msg.mcdr.car; c1.y = l_msg.mcdr.cdr;     \
      update_ref_msg(c1, 1);                            \
    }                                                   \
    DEBUG("enqueue from LAMBDA_APP\n");                 \
    enqueue(msg);                                       \
    break;                                              \
  }
#define PRIMOPT_APP(op, arg) { op.cdr = arg; op.hdr = CURRIED; }
#define CURRIED_APP(op, arg) {                                     \
    switch(op.car.car){                                            \
    case PLUS:   op.cdr.car = op.car.cdr + arg.car; break;         \
    case MINUS:  op.cdr.car = op.car.cdr - arg.car; break;         \
    case TIMES:  op.cdr.car = op.car.cdr * arg.car; break;         \
    case DIVIDE: op.cdr.car = op.car.cdr / arg.car; break;         \
    case EQUAL:                                                    \
      if(op.car.cdr == arg.car) op.cdr.car = TRUE;                 \
      else op.cdr.car = FALSE;                                     \
      break;                                                       \
    case LESS:                                                     \
      if(op.car.cdr < arg.car) op.cdr.car = TRUE;                  \
      else op.cdr.car = FALSE;                                     \
      break;                                                       \
    default: ERROR("unsupported CURRIED op.careration"); break;    \
    }                                                              \
    if(op.car.car <= DIVIDE) op.cdr.hdr = INTEGER;                 \
    else                     op.cdr.hdr = BOOLEAN;                 \
    op.car.hdr  = UNPACK;                                          \
  }
#define UNPACK_APP(op, arg) {                                     \
  DEBUG2("    NIL: unpacking from (%d,%d)\n", arg.x, arg.y);      \
  op = AT(arg).cdr;                                               \
  if(AT(arg).cdr.hdr == LOCAL){                                   \
    COORD_OF_PTR(arg, AT(arg).cdr);                               \
    update_ref_msg(arg, 1);                                       \
  }                                                               \
  update_ref_msg(arg, -1); }
#define BOOLEAN_APP(place, ptr, t1, t2, i1) {                   \
    DEBUG2("    NIL: bool replacement at (%d,%d)\n",            \
           place.x, place.y);                                   \
    t1 = open_space(place);                                     \
    AT(t1).refs = 1;                                            \
    i1 = ptr.car;                                               \
    ptr.hdr = LOCAL;                                            \
    ptr.car = t1.x;                                             \
    ptr.cdr = t1.y;                                             \
    t2 = open_space(t1);                                        \
    AT(t2).refs = 1;                                            \
    AT(t1).car.hdr = LAMBDA;                                    \
    AT(t1).car.car = TRUE;                                      \
    AT(t1).cdr.hdr = LOCAL;                                     \
    AT(t1).cdr.car = t2.x;                                      \
    AT(t1).cdr.cdr = t2.y;                                      \
    t1 = open_space(t2);                                        \
    AT(t1).refs = 1;                                            \
    AT(t2).car.hdr = LAMBDA;                                    \
    AT(t2).car.car = FALSE;                                     \
    AT(t2).cdr.hdr = LOCAL;                                     \
    AT(t2).cdr.car = t1.x;                                      \
    AT(t2).cdr.cdr = t1.y;                                      \
    AT(t1).car.hdr = SYMBOL;                                    \
    AT(t1).car.car = i1;                                        \
    AT(t1).cdr.hdr = NIL;                                       \
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

/* duplicate a linked structure */
void duplicate(coord from, coord to);

/* try to apply a message from the queue */
void run_queue();

/* send a message to PLACE to change its ref counter by DIFF */
void update_ref_msg(coord place, int diff);

/* run the onc at PLACE according to its contents and message */
void run(coord place);
