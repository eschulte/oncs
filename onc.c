// TODO: lookup how to do a quick binary-string-keyed lookup table in
//       which most actions are (hopefully) bit operations

/* Message */
struct message {
  int address;
  int payload;
};

/* ONC cell */
struct onc {
  // TODO: these are ints but should really be byte arrays
  int hdr;                      /* header informations */
  int car;                      /* car pointer */
  int cdr;                      /* cdr pointer */
  int env;                      /* env pointer */
  int bay;                      /* holds messages and workspace */
};

void process(onc * onc){
  /* (1) process any messages in the message bay */
  /* (2) if evaluable, evaluate depending on contents of car and cdr */
  /* (3) send outgoing messages */
}
