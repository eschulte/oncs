#include <string.h>
#include <stdarg.h>
#include "../oncs.h"

/* global state */
extern int verbose_p;
extern int fail_p;

/* setup */
void init(int argc, char *argv[]);

/* logging */
int debug(const char *format, ...);

/* world building */
void place_lambda(coord place, int variable);
void place_symbol(coord place, int variable);
void place_cdr_local(coord place, coord local);
