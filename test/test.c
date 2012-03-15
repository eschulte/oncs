#include "test.h"

int verbose_p = 0;
int fail_p = 0;

void init(int argc, char *argv[]){
  int i;
  for(i=1;i<argc;i++)
    if(strcmp(argv[i], "-v") == 0)
      verbose_p=1;
}

int debug(const char *format, ...){
    va_list args;
    va_start(args, format);
    if(verbose_p) vprintf(format, args);
    va_end(args);
}

void place_lambda(coord place, int variable){
  AT(place).refs = 1;           /* mark space as full */
  AT(place).car.hdr = variable; /* LAMBDA 32 */
}

void place_cdr_local(coord place, coord local){
  AT(place).refs = 1;           /* mark space as full */
  AT(place).cdr.hdr = 1;        /* LOCAL */
  AT(place).cdr.car = local.x;  /* x */
  AT(place).cdr.cdr = local.y;  /* y */
}

void place_symbol(coord place, int variable){
  AT(place).refs = 1;           /* mark space as full */
  AT(place).car.hdr = 3;        /* SYMBOL */
  AT(place).car.car = 32;       /* 32 */
}
