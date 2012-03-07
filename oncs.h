/* types of pointers */
#define NIL     0
#define LOCAL   1
#define INTEGER 2
#define SYMBOL  3
#define LAMBDA  4

/* world size and access */
#define SIZE 100
#define AT(point) world[point.x][point.y]
#define WRAP(x) x%SIZE

/* operations and control flow */
#define COPY_PTR(x,y) x.hdr = y.hdr; x.car = y.car; x.cdr = y.cdr;

/* structures inhabiting the world */
typedef struct { int x, y; } coord;
typedef struct { int hdr, car, cdr; } ptr;
typedef struct { ptr var, car, cdr; } msg;
typedef struct { ptr car, cdr, msg; int refs; } onc;

/* return the coord of the nearest open space */
coord open_space(coord place);

/* duplicate a pointer */
void duplicate_ptr(ptr from, ptr to, coord from_coord, coord to_coord);

/* duplicate a linked structure */
void duplicate(coord from, coord to);

/* traverse structure at COORD replacing FROM with TO */
void replace_ptr(int var, ptr to, coord to_coord, ptr new, coord new_coord);

/* run the onc at PLACE according to its contents and message */
void run(coord place);

/* which functions call which functions
 * ====================================
 *
 *    open_space :
 * duplicate_ptr : open_space
 *     duplicate : duplicate_ptr
 *   replace_ptr : duplicate_ptr
 *           run : replace
 *
 */
