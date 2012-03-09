#include <stdlib.h>
#include <stdio.h>
#include "../oncs.h"

int main(int argc, char* argv){
  /* test the open_space function */
  int i, j;
  coord place, new;
  place.x = place.y = 2;
  AT(place).refs = 1;
  AT(place).car.hdr = 32;       /* LAMBDA 32 */
  AT(place).cdr.hdr = 1;        /* LOCAL */
  AT(place).cdr.car = 1;
  AT(place).cdr.cdr = 0;
  place.x = 3;
  AT(place).refs = 1;
  AT(place).car.hdr = 3;        /* SYMBOL */
  AT(place).car.car = 32;       /* 32 */
  for(i=0;i<97;i++){
    new = open_space(place);
    AT(new).refs = 1;
    show_world();
  }
  /* should be one empty space */
  for(i=0;i<SIZE;i++)
    for(j=0;j<SIZE;j++)
      if(world[j][i].refs == 0)
        break;
  if(i == (SIZE-1) && j == (SIZE-1))
    exit(1);
  new = open_space(place);
  AT(new).refs = 1;
  show_world();
  /* should be no more empty spaces */
  for(i=0;i<SIZE;i++)
    for(j=0;j<SIZE;j++)
      if(world[j][i].refs == 0)
        exit(1);
  return 0;
}
