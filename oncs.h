/* types of pointers */
#define NIL     0
#define LOCAL   1
#define INTEGER 2
#define SYMBOL  3
#define LAMBDA  4

/* world size and access */
#define SIZE 100
#define AT(point) world[point.x][point.y]

/* operations and control flow */
#define EXIT(msg) printf(msg); goto bail;
#define COPY_PTR(x,y) x.hdr = y.hdr; x.car = y.car; x.cdr = y.cdr;

/* structures inhabiting the world */
typedef struct { int x, y; } coord;
typedef struct { int hdr, car, cdr; } ptr;
typedef struct { ptr var, car, cdr; } msg;
typedef struct { ptr car, cdr, msg; int refs; } onc;

/* return the coord of the nearest open space */
coord open_space(coord place);

/* duplicate a pointer */
void duplicate_ptr(ptr from, ptr to, coord to_coord);

/* duplicate a linked structure */
void duplicate(coord from, coord to);

/* traverse structure at COORD replacing FROM with TO */
void replace(coord place, coord from, coord to);

/* run the onc at PLACE according to its contents and message */
void run(coord place);

/* which functions call which functions
 * ====================================
 *
 *    open_space :
 * duplicate_ptr : open_space
 *     duplicate : duplicate_ptr
 *       replace : duplicate
 *           run : replace
 *
 */
