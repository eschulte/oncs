#include <stdio.h>

/* ONC */
struct onc        // +----------------+
{                 // |      ONC       |
  long long hdr;  // | header    byte |
  long long env;  // | env       word |
  long long msg;  // | message   word |
  long long car;  // | car       word |
  long long cdr;  // | cdr       word |
};                // +----------------+

/* Message Headers */
typedef unsigned char header;

/* Message Payloads */
typedef unsigned long long word;

void process_header(struct onc self, header head){
  switch( head ){
  case 7: /* 1 1 1 0 -- direct to value of payload in env */    break;
  case 3: /* 1 1 0 0 -- direct to local direction of payload */ break;
  case 5: /* 1 0 1 0 -- direct to value of car */               break;
  case 1: /* 1 0 0 0 -- direct to value of cdr */               break;
  case 6: /* 0 1 1 0 -- store payload in message */             break;
  case 2: /* 0 1 0 0 -- store payload in env */                 break;
  case 4: /* 0 0 1 0 -- store payload in car */                 break;
  case 0: /* 0 0 0 0 -- store payload in cdr */                 break;
  }
}

int main(){
  printf("ONCs are not Conses\n");
  return 0;
}
