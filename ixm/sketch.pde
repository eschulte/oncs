/* ONCS -- A distributed lisp virtual machine        -*- mode:C++ -*-
 *
 * Copyright (C) 2012  Eric Schulte
 *
 * License: GPLV3
 */
#include "../src/oncs.h"

int loop_delay = 1000;
void loop() {
  delay(loop_delay);
  // blink green if seen this before
  QLED.on(BODY_RGB_GREEN_PIN, 500);
  QLED.off(BODY_RGB_GREEN_PIN, 500);
}

#define SFB_SKETCH_CREATOR_ID B36_3(e,m,s)
#define SFB_SKETCH_PROGRAM_ID B36_4(o,n,c,s)
#define SFB_SKETCH_COPYRIGHT_NOTICE "GPL V3"
