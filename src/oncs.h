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
#define DUPLICATE_LOCAL(place, near, tmp_from, tmp_to) { \
    tmp_from.x = place.car;                              \
    tmp_from.y = place.cdr;                              \
    tmp_to = open_space(near);                           \
    duplicate(tmp_to, tmp_from); }

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

/* traverse structure at COORD replacing FROM with TO */
void replace_ptr(int var, ptr to, coord to_coord, ptr new, coord new_coord);

/* send a message to a place */
void message(coord place, msg msg);

/* run the onc at PLACE according to its contents and message */
void run(coord place);

/* which functions call which functions
 * ====================================
 *    open_space :
 *     duplicate :
 *   replace_ptr :
 *       message :
 *           run : replace message
 */
