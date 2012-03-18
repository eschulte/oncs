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


void show_ptr(ptr ptr){
  switch(ptr.hdr){
  case NIL:     printf("_"); break;
  case LOCAL:   printf("^"); break;
  case INTEGER: printf("i"); break;
  case SYMBOL:  printf("s"); break;
  case LAMBDA:  printf("l"); break;
  default:      printf("?"); break;
  }
}

void show_world(){
  onc tmp;
  int i, j;
  printf("\n");
  fflush(stdout);
  for(i=0;i<SIZE;i++) {
    for(j=0;j<SIZE;j++) {
      tmp = world[j][i];
      if(tmp.refs > 0) {
        printf("["); show_ptr(tmp.mcar);
        printf(","); show_ptr(tmp.car);
        printf(","); show_ptr(tmp.cdr);
        printf("]");
      } else printf("       ");
      if(j<(SIZE-1)) printf(" ");
    }
    printf("\n");
    fflush(stdout);
  }
}

int population(){
  int i, j, count;
  count = 0;
  for(i=0;i<SIZE;i++)
    for(j=0;j<SIZE;j++)
      if (world[j][i].refs > 0)
        count++;
  return count;
}
