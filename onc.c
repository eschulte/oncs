#include <stdio.h>

/* see https://en.wikipedia.org/wiki/Bitfield */
typedef unsigned char header;
#define first  (1 << 0)          /* 0x01 */
#define second (1 << 1)          /* 0x01 */
#define third  (1 << 2)          /* 0x02 */
#define fourth (1 << 3)          /* 0x03 */

header header_new(void){ return 0; }
void   header_set(header *self, int flag){ *self |= flag;  }
void header_reset(header *self, int flag){ *self &= ~flag; }
void   header_rev(header *self, int flag){ *self ^= flag;  }
int    header_get(header *self, int flag){ return (*self & flag) != 0; }

void display(header head){
  printf("%d %d %d %d %d\n", head,
         header_get(&head, first),
         header_get(&head, second),
         header_get(&head, third),
         header_get(&head, fourth));
}

void process_header(header head){
  switch( head ){
  case 7:                       /* 1 1 1 0 */
    /* look up variable in env and direct to that location */
    break; 
  case 3:                       /* 1 1 0 0 */
    /* send message in local direction */
    break; 
  case 5:                       /* 1 0 1 0 */
    /* look up value in car and direct there */
    break; 
  case 1:                       /* 1 0 0 0 */
    /* look up value in cdr and direct there */ 
   break; 
  case 6:                       /* 0 1 1 0 */
    /* store subsequent payload in message */
    break; 
  case 2:                       /* 0 1 0 0 */
    /* store subsequent payload in env */
    break; 
  case 4:                       /* 0 0 1 0 */
    /* store subsequent payload in car */
    break; 
  case 0:                       /* 0 0 0 0 */
    /* store subsequent payload in cdr */
    break; 
  }
}

int main(){
  header head = header_new();
  header_set(&head, second);
  display(head);
  process_header(head);
  return 0;
}

/* /\* Message *\/ */
/* struct message { */
/*   int address; */
/*   int payload; */
/* }; */

/* /\* ONC cell *\/ */
/* struct onc { */
/*   // TODO: these are ints but should really be byte arrays */
/*   int hdr;                      /\* header informations *\/ */
/*   int car;                      /\* car pointer *\/ */
/*   int cdr;                      /\* cdr pointer *\/ */
/*   int env;                      /\* env pointer *\/ */
/*   int bay;                      /\* holds messages and workspace *\/ */
/* }; */

/* void process(onc * onc){ */
/*   /\* (1) process any messages in the message bay *\/ */
/*   /\* (2) if evaluable, evaluate depending on contents of car and cdr *\/ */
/*   /\* (3) send outgoing messages *\/ */
/* } */
