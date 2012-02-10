#define SIZE 1000

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

/* pop direction from payload, push onto end and return */
unsigned int pop_direction(word * payload);

/* accept message header updating the ONC header */
void accept_message_header
(struct onc self, header head, unsigned int x, unsigned int y);

/* accept message payload saving or passing through */
void accept_message_payload
(struct onc self, word payload, unsigned int x, unsigned int y){
