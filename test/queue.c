#include "test.h"

int main(int argc, char *argv[]){
  init(argc, argv);
  debug(2, "starting queue test\n");

  msg msg;
  msg.coord.x = msg.coord.y = 0;
  msg.mcar.hdr = SYMBOL;
  msg.mcar.car = 1;

  /* enqueue a message */
  debug(2, "calling enqueue\n");
  enqueue(msg);

  /* should be in the queue */
  SHOULD(queue_population() == 1);
  debug(1, "qbeg=%d qend=%d queue_population=%d fail_p=%d\n",
        qbeg, qend, queue_population(), fail_p);

  /* change the value of the instance not in the queue */
  msg.mcar.hdr = NIL;

  /* dequeue a message */
  debug(2, "calling dequeue\n");
  msg = dequeue();

  /* should be empty */
  SHOULD(queue_population() == 0);
  debug(1, "qbeg=%d qend=%d queue_size=%d fail_p=%d\n",
        qbeg, qend, queue_population(), fail_p);

  /* should have the initial value (stored in the queue) */
  SHOULD(msg.mcar.hdr == SYMBOL);

  /* return indicates success or failure */
  return fail_p;
}
