#include <stdio.h>

/* types of pointers */
#define NIL     0
#define LOCAL   1
#define INTEGER 2
#define SYMBOL  3
#define LAMBDA  4

/* world size */
#define SIZE 100

typedef struct { int hdr, car, cdr; } ptr;
typedef struct { ptr var, car, cdr; } msg;
typedef struct { ptr car, cdr, msg; } onc;

onc world[SIZE][SIZE];

int main(int argc, char* argv){
  printf("oncs are not conses: ");
  world[0][0].car.hdr = 3;
  world[0][0].car.car = 0;
  world[0][0].cdr.hdr = 0;
  printf(" world[0][0].hdr=%d\n", world[0][0].car.hdr);
}

/* return the free variables in an expression */
int* free_variables(ptr var){ }

onc duplicate(onc){
  if(get_free_space()) {
    /* fill this onc */
    /* propagate */
  } else {
    printf("ERROR: can't allocate free space.\n");
  }
}

/* propagate lambda-msg in onc.msg,  */
void handle_lambda_message(onc onc){
  if (onc.car.hdr == SYMBOL && onc.car.car == onc.msg.var) {
    /* replace matching symbol */
    onc.car.car = duplicate(onc.msg.car);
  } else if (onc.car.hdr == LAMBDA && onc.car.car == onc.msg.var) {
    /* skip shadowed var */
  } else if (onc.car.hdr == NIL) {
    
  }
  // if car == var, then replace with arg
  if (onc.car.hdr == 3 && onc.car.car == onc.msg.var)
    onc.car
    replace(, onc.msg.cdr);
  }
}
