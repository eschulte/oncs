#define SIZE 1000
#define first  (1 << 0)          /* 0x01 */
#define second (1 << 1)          /* 0x01 */
#define third  (1 << 2)          /* 0x02 */
#define fourth (1 << 3)          /* 0x03 */

/* Headers -- one type for both messages and oncs */
typedef unsigned char header;

/* Message Payloads */
typedef unsigned long long word;

/* ONC */
struct onc        // +----------------+
{                 // |      ONC       |
  header hdr;     // | header    byte |
  long long env;  // | env       word |
  long long msg;  // | message   word |
  long long car;  // | car       word |
  long long cdr;  // | cdr       word |
};                // +----------------+

/* bit twiddling */
int  header_get(header *self, int flag);
void header_set(header *self, int flag);
void header_clr(header *self, int flag);
void header_rev(header *self, int flag);
int  word_get(word *self, int flag);
void word_set(word *self, int flag);
void word_clr(word *self, int flag);
void word_rev(word *self, int flag);

/* pop direction from payload, push onto end and return */
unsigned char pop_direction(word * payload);

/* accept message header updating the ONC header */
void accept_message_header
(struct onc self, header head, unsigned int x, unsigned int y);

/* accept message payload saving or passing through */
void accept_message_payload
(struct onc self, word payload, unsigned int x, unsigned int y);
