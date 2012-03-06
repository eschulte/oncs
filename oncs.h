/* types of pointers */
#define NIL     0
#define LOCAL   1
#define INTEGER 2
#define SYMBOL  3
#define LAMBDA  4

/* world size and access */
#define SIZE 100
#define AT(point) world[point.x][point.y]

/* control flow */
#define EXIT(msg) printf(msg); goto bail;

/* structures inhabiting the world */
typedef struct { int x, y; } coord;
typedef struct { int hdr, car, cdr; } ptr;
typedef struct { ptr var, car, cdr; } msg;
typedef struct { ptr car, cdr, msg; int refs; } onc;

/* return the coord of the nearest open space */
coord open_space(coord place);

/* duplicate a linked structure */
coord duplicate(coord from, coord to);

/* traverse structure at COORD replacing FROM with TO */
coord replace(coord place, coord from, coord to);

/* run the onc at PLACE according to its contents and message */
void run(coord place);
