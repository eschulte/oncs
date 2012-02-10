#include <stdio.h>
#define SIZE 1000
#define QUEBITS 64064 /* TODO: assoc onc with every payload & header?  */

/* ONC */
struct onc        // +----------------+
{                 // |      ONC       |
  header hdr;     // | header    byte |
  long long env;  // | env       word |
  long long msg;  // | message   word |
  long long car;  // | car       word |
  long long cdr;  // | cdr       word |
};                // +----------------+

/* Message Headers */
typedef unsigned char header;

/* Message Payloads */
typedef unsigned long long word;

/* Message Queue -- TODO: this shouldn't be global, just getting started */
uchar mque[QUEBITS];

void accept_message_header(struct onc self, header head){
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

void accept_message_payload(struct onc self, word payload){
  switch( head ){
  case 7: /* 1 1 1 0 -- direct to value of payload in env */    break;
  case 3: /* 1 1 0 0 -- direct to local direction of payload */ break;
  case 5: /* 1 0 1 0 -- direct to value of car */               break;
  case 1: /* 1 0 0 0 -- direct to value of cdr */               break;
  case 6: /* 0 1 1 0 */ onc.msg = payload;                      break;
  case 2: /* 0 1 0 0 */ onc.env = payload;                      break;
  case 4: /* 0 0 1 0 */ onc.car = payload;                      break;
  case 0: /* 0 0 0 0 */ onc.cdr = payload;                      break;
  }
}

struct onc medium[SIZE][SIZE];

int main(int argc, const char* argv[]){
  int steps = atoi(argv[1]);
  int step, x, y;
  for(step=0; step<=steps; step++){
    for(x=0; x<SIZE; x++){
      for(y=0; y<SIZE; y++){
        
      }
    }
  }
  return 0;
}
