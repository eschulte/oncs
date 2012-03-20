#include "test.h"

int queue_size(){ return (qend-qbeg) % QLENGTH; }

int main(int argc, char *argv[]){
  init(argc, argv);

  msg msg;
  msg.coord.x = msg.coord.y = 0;
  msg.mcar.hdr = SYMBOL;
  msg.mcar.car = 1;

  /* enqueue a message */
  enqueue(msg);

  /* should be in the queue */
  SHOULD(queue_size() == 1);
  debug(1, "qbeg=%d qend=%d queue_size=%d fail_p=%d\n",
        qbeg, qend, queue_size(), fail_p);

  /* change the value of the instance not in the queue */
  msg.mcar.hdr = NIL;

  /* dequeue a message */
  msg = dequeue();

  /* should be empty */
  SHOULD(queue_size() == 0);
  debug(1, "qbeg=%d qend=%d queue_size=%d fail_p=%d\n",
        qbeg, qend, queue_size(), fail_p);

  /* should have the initial value (stored in the queue) */
  SHOULD(msg.mcar.hdr == SYMBOL);

  /* return indicates success or failure */
  return fail_p;
}
