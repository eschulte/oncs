#include <stdio.h>

/* Message Headers */
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
void print_header(header head){ printf("%d %d %d %d %d\n", head,
                                       header_get(&head, first),
                                       header_get(&head, second),
                                       header_get(&head, third),
                                       header_get(&head, fourth)); }

/* Message Payloads */
typedef unsigned long long word;

/* ONC */
struct onc {
  long long hdr;
  long long car;
  long long cdr;
  long long env;
  long long msg;
};

void message_to(header addr) {
  /* Maybe just, place the location into the local message buffer, and
     then flip something in the header which means that all subsequent
     incoming messages go in the same direction. */
}

void env_set(struct onc onc, word value){ } /* Set value into env of onc */
void car_set(struct onc onc, word value){ } /* Set value into car of onc */
void cdr_set(struct onc onc, word value){ } /* Set value into cdr of onc */

void process_header(struct onc self, header head){
  switch( head ){
  case 7:                       /* 1 1 1 0 */
    /* look up variable in env and direct to that location */
    perror("variable reference not supported\n");
    break;
  case 3:                       /* 1 1 0 0 */
    /* send message in local direction */
    /* save the local direction into message */
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
  printf("ONCs are not Conses\n");
  return 0;
}
