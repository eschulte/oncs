#include <string.h>
#include <stdarg.h>
#include "../oncs.h"

#define SHOULD(x) if(!(x)) fail_p=1;

/* global state */
extern int verbose_p;
extern int fail_p;

/* setup */
void init(int argc, char *argv[]);

/* logging */
int debug(const char *format, ...);

/* world queries */
void show_ptr(ptr ptr);
void show_world();
int population();

/* world building */
void place_lambda(coord place, int variable);
void place_symbol(coord place, int variable);
void place_cdr_local(coord place, coord local);
