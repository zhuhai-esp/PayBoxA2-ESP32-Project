#include "wt32_lv.h"
#include "wt32_wifi.h"
#include <Arduino.h>
#include <demos/lv_demos.h>

long check1s = 0;
long check5ms = 0;

void inline startConfigTime() {
  const int timeZone = 8 * 3600;
  configTime(timeZone, 0, "ntp6.aliyun.com", "cn.ntp.org.cn", "ntp.ntsc.ac.cn");
  while (time(nullptr) < 8 * 3600 * 2) {
    delay(500);
  }
}

void setup() {
  Serial.begin(115200);
  init_wt32_lvgl();
  lv_demo_widgets();
  autoConfigWifi();
  startConfigTime();
  setupOTAConfig();
}

void loop() {
  auto ms = millis();
  if (ms - check1s > 1000) {
    check1s = ms;
    ArduinoOTA.handle();
  }
  if (ms - check5ms > 5) {
    check5ms = ms;
    lv_timer_handler();
  }
}