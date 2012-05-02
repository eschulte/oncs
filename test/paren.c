#include "test.h"
#define BUF_SIZE 1024

int main(int argc, char *argv[]){
  char buf1[] = "(1234)";
  char buf2[] = "(a (b c) d (()))";
  init(argc, argv);
  SHOULD(close_paren(buf1, 0) == 5);
  SHOULD(close_paren(buf2, 0) == 15);
  SHOULD(close_paren(buf2, 3) == 7);
  
  /* return indicates success or failure */
  return fail_p;
}
