#include <stdio.h>
#define SIZE 100

typedef struct { int hdr, car, cdr; } ptr;
typedef struct { ptr var, arg, tkn; } msg;
typedef struct { ptr car, cdr, msg; } onc;

onc world[SIZE][SIZE];

int main(int argc, char* argv){
  printf("oncs are not conses: ");
  world[0][0].car.hdr = 3;
  world[0][0].car.car = 0;
  world[0][0].cdr.hdr = 0;
  printf(" world[0][0].hdr=%d\n", world[0][0].car.hdr);
}

void handle_lambda_message(onc self){ /* propagate lambda-msg in onc.msg,  */ }
