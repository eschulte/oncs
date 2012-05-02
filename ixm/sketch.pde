/* ONCS -- A distributed lisp virtual machine        -*- mode:C++ -*-
 *
 * Copyright (C) 2012  Eric Schulte
 *
 * License: GPLV3
 */
#include "../src/oncs.h"

int loop_delay = 1000;

void setup(){ QLED.begin(); }

void loop() {
  delay(loop_delay);
  QLED.on(BODY_RGB_BLUE_PIN, 500);
  QLED.off(BODY_RGB_BLUE_PIN, 500);
}

#define SFB_SKETCH_CREATOR_ID B36_3(e,m,s)
#define SFB_SKETCH_PROGRAM_ID B36_4(o,n,c,s)
#define SFB_SKETCH_COPYRIGHT_NOTICE "GPL V3"
