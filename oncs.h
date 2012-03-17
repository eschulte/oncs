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

/* print the world to STDOUT */
void show_world();

/* return the coord of the nearest open space */
coord open_space(coord place);

/* duplicate a pointer */
void duplicate_ptr(ptr from, ptr to, coord from_coord, coord to_coord);

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
 *      show_ptr :
 *    show_world : show_ptr
 *    open_space :
 * duplicate_ptr : open_space
 *     duplicate : duplicate_ptr
 *   replace_ptr : duplicate_ptr
 *       message : 
 *           run : replace message
 */
