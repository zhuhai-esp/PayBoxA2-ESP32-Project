#ifndef __JPG_MAP_H__
#define __JPG_MAP_H__

#include <Arduino.h>
#include <TJpg_Decoder.h>

#include "img/pangzi/i0.h"
#include "img/pangzi/i1.h"
#include "img/pangzi/i2.h"
#include "img/pangzi/i3.h"
#include "img/pangzi/i4.h"
#include "img/pangzi/i5.h"
#include "img/pangzi/i6.h"
#include "img/pangzi/i7.h"
#include "img/pangzi/i8.h"
#include "img/pangzi/i9.h"

#include "img/humidity.h"
#include "img/temperature.h"
#include "img/watch_bottom.h"
#include "img/watch_top.h"

void inline imgAnim() {
  int x = 104, y = 94, dt = 30;
  TJpgDec.drawJpg(x, y, i0, sizeof(i0));
  delay(dt);
  TJpgDec.drawJpg(x, y, i1, sizeof(i1));
  delay(dt);
  TJpgDec.drawJpg(x, y, i2, sizeof(i2));
  delay(dt);
  TJpgDec.drawJpg(x, y, i3, sizeof(i3));
  delay(dt);
  TJpgDec.drawJpg(x, y, i4, sizeof(i4));
  delay(dt);
  TJpgDec.drawJpg(x, y, i5, sizeof(i5));
  delay(dt);
  TJpgDec.drawJpg(x, y, i6, sizeof(i6));
  delay(dt);
  TJpgDec.drawJpg(x, y, i7, sizeof(i7));
  delay(dt);
  TJpgDec.drawJpg(x, y, i8, sizeof(i8));
  delay(dt);
  TJpgDec.drawJpg(x, y, i9, sizeof(i9));
  delay(dt);
}

#endif