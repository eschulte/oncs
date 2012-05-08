#include "test.h"

int main(int argc, char *argv[]){
  char* bits;
  int i, j;
  coord place, new;

  init(argc, argv);

  /* setup */
  new.x = 3;
  new.y = place.x = place.y = 2;
  AT(place).refs++;
  AT(new).refs++;
  LAMBDA_SET(place, 32);
  LOCAL_SET(place, cdr, bits, new);
  SYMBOL_SET(new, car, 32);

  /* fill up the world */
  for(i=0;i<(SIZE*SIZE-3);i++){
    new = open_space(place);
    AT(new).refs = 1;
    debug(1, "\n");
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
  debug(1, "\n");
  show_world();
  /* should be no more empty spaces */
  for(i=0;i<SIZE;i++)
    for(j=0;j<SIZE;j++)
      if(world[j][i].refs == 0)
        fail_p=1;
  /* return indicates success or failure */
  return fail_p;
}
