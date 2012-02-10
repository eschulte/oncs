#include <stdio.h>
#include "onc.h"

/* Field of ONCs */
struct onc oncs[SIZE][SIZE];

/* Local Message Queues -- one per ONC */
long long messages[SIZE][SIZE];
unsigned int message_types[SIZE][SIZE];

int main(int argc, const char* argv[]){
  int steps, step, x, y;
  if(argc > 1)
    steps = atoi(argv[1]);
  else
    steps = 0;
  for(step=0; step<=steps; step++){
    for(x=0; x<SIZE; x++){
      for(y=0; y<SIZE; y++){

      }
    }
  }
  printf("ONCs are not cons\n");
  return 0;
}

void accept_message_header
(struct onc self, header head, unsigned int x, unsigned int y){
  unsigned int direction;
  switch( head ){
  case 7: /* 1 1 1 0 -- direct to value of payload in env */    break;
  case 3: /* 1 1 0 0 -- direct to local direction of payload */ break;
  case 5: /* 1 0 1 0 -- direct to value of car */
    
    break;
  case 1: /* 1 0 0 0 -- direct to value of cdr */               break;
  case 6: /* 0 1 1 0 -- store payload in message */             break;
  case 2: /* 0 1 0 0 -- store payload in env */                 break;
  case 4: /* 0 0 1 0 -- store payload in car */                 break;
  case 0: /* 0 0 0 0 -- store payload in cdr */                 break;
  }
}

void accept_message_payload
(struct onc self, word payload, unsigned int x, unsigned int y){
  switch( self.hdr ){
  case 7: /* 1 1 1 0 -- direct to value of payload in env */    break;
  case 3: /* 1 1 0 0 -- direct to local direction of payload */ break;
  case 5: /* 1 0 1 0 -- direct to value of car */               break;
  case 1: /* 1 0 0 0 -- direct to value of cdr */               break;
  case 6: /* 0 1 1 0 */ self.msg = payload;                     break;
  case 2: /* 0 1 0 0 */ self.env = payload;                     break;
  case 4: /* 0 0 1 0 */ self.car = payload;                     break;
  case 0: /* 0 0 0 0 */ self.cdr = payload;                     break;
  }
}

/* pop direction from payload, push onto end and return */
unsigned int pop_direction(word * payload){
  
}
