/* Copyright (C) 2012 Eric Schulte */
#include <stdlib.h>
#include <stdio.h>

/* types of pointers */
#define NIL     0
#define LOCAL   1
#define INTEGER 2
#define SYMBOL  3
#define LAMBDA  4

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
#define LAMBDA_APP(where, msg, t1, t2)                          \
  switch(where.hdr){                                            \
  case NIL:                                                     \
  case INTEGER: break;                                          \
  case SYMBOL:                                                  \
    if(AT(place).mcar.car == where.car){                        \
      DUPLICATE_LOCAL(AT(place).mcdr, place, t1, t2);           \
      where.hdr = LOCAL;                                        \
      where.car = t2.x;                                         \
      where.cdr = t2.y;                                         \
    }                                                           \
    break;                                                      \
  case LAMBDA:                                                  \
    if(AT(place).mcar.car == where.car) return;                 \
    break;                                                      \
  case LOCAL:                                                   \
    msg.coord.x = where.car;                                    \
    msg.coord.y = where.cdr;                                    \
    msg.mcar = AT(place).mcar;                                  \
    msg.mcdr = AT(place).mcdr;                                  \
    t1.x = msg.mcdr.car; t1.y = msg.mcdr.cdr;                   \
    update_ref_msg(t1, 1);                                      \
    enqueue(msg);                                               \
    break;                                                      \
  }

/* utility macros */
#define DEBUG_P 0
#define DEBUG(fmt) if(DEBUG_P) printf(fmt);
#define DEBUG1(fmt, x) if(DEBUG_P) printf(fmt, x);
#define DEBUG2(fmt, x, y) if(DEBUG_P) printf(fmt, x, y);
#define ERROR(message) { printf("ERROR: %s\n", message); exit(1); }

/* structures inhabiting the world */
typedef struct { int x, y; } coord;
typedef struct { int hdr, car, cdr; } ptr;
typedef struct { ptr mcar,mcdr; coord coord; } msg;
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
