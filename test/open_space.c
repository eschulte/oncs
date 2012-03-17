#include "test.h"

int main(int argc, char *argv[]){
  init(argc, argv);

  /* setup */
  int i, j;
  coord place, new;
  new.x = 3;
  new.y = place.x = place.y = 2;
  place_lambda(place, 32);
  place_cdr_local(place, new);
  place_symbol(new, 32);

  /* fill up the world */
  for(i=0;i<97;i++){
    new = open_space(place);
    AT(new).refs = 1;
    if(verbose_p) show_world();
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
  if(verbose_p) show_world();
  /* should be no more empty spaces */
  for(i=0;i<SIZE;i++)
    for(j=0;j<SIZE;j++)
      if(world[j][i].refs == 0)
        fail_p=1;
  /* return indicates success or failure */
  return fail_p;
}