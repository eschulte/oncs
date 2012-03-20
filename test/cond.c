#include "test.h"
#define BUF_SIZE 1024

void full_run(char *expr){
  debug(2, "1\n");
  coord place;
  place.x = place.y = 4;
  clear_world();
  string_to_onc(place, expr, 0);
  show_world();
  run(place);
  show_world();
  debug(2, "2\n");
  do{
    place = queue[qbeg].coord; debug(2, "3\n");
    if(queue_population() > 0){
      debug(2, "queue.length%d\n", queue_population());
      run_queue(); debug(2, "4\n"); show_world(); debug(2, "5\n");
      run(place);  debug(2, "6\n"); show_world(); debug(2, "7\n");
    }
    printf("8\n");
  } while(queue_population() > 0);
  place.x = place.y = 4;
  debug(1, "(%d,%d):%s\n", place.x, place.y, expr);
}

int main(int argc, char *argv[]){
  init(argc, argv);

  char expr1[] = "(#L1 (#L2 #S1)) 8";
  full_run(expr1);

  /* char expr2[] = "(#L1 (#L2 #S1)) ((#L1 (#L2 #S1)) (3 4))"; */
  /* full_run(expr2); */

  /* return indicates success or failure */
  return fail_p;
}
