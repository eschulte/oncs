/* ONCS -- A distributed lisp virtual machine        -*- mode:C++ -*-
 *
 * Copyright (C) 2012  Eric Schulte
 *
 * License: GPLV3
 */
#include "../test/test.h"

int loop_delay = 1000;

void setup(){ QLED.begin(); }

void loop() {
  msg msg;
  delay(loop_delay);
  QLED.on(BODY_RGB_BLUE_PIN, 500);
  if(queue_population() > 0){
    msg = queue[qbeg];
    /* only pop from queue if dest. is empty */
    if(AT(msg.place).mcar.hdr == NIL){
      msg = dequeue();
      AT(msg.place).mcar = msg.mcar;
      AT(msg.place).mcdr = msg.mcdr;
    }
    /* run destination either way */
    run(msg.place);
  } else { run_one(); }
  QLED.off(BODY_RGB_BLUE_PIN, 500);
}

#define SFB_SKETCH_CREATOR_ID B36_3(e,m,s)
#define SFB_SKETCH_PROGRAM_ID B36_4(o,n,c,s)
#define SFB_SKETCH_COPYRIGHT_NOTICE "GPL V3"
