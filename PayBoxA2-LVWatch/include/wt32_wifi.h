#ifndef __WT32_WIFI_H__
#define __WT32_WIFI_H__

#include "wt32_lv.h"
#include <Arduino.h>
#include <ArduinoOTA.h>

#define TZ 8            // 中国时区为8
#define DST_MN 0        // 默认为0
#define TZ_MN ((TZ)*60) // 时间换算
#define TZ_SEC ((TZ)*3600)
#define DST_SEC ((DST_MN)*60)

char buf[48] = {0};

void inline autoConfigWifi() {
  WiFi.mode(WIFI_MODE_STA);
  WiFi.begin();
  for (int i = 0; WiFi.status() != WL_CONNECTED && i < 100; i++) {
    delay(100);
  }
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.mode(WIFI_MODE_APSTA);
    WiFi.beginSmartConfig();
    while (WiFi.status() != WL_CONNECTED) {
      delay(100);
    }
    WiFi.stopSmartConfig();
    WiFi.mode(WIFI_MODE_STA);
  }
}

void inline setupOTAConfig() {
  ArduinoOTA.onStart([] { show_center_msg("OTA Update"); });
  ArduinoOTA.onProgress([](u32_t pro, u32_t total) {
    sprintf(buf, "OTA Updating: %d / %d", pro, total);
    show_center_msg(buf);
  });
  ArduinoOTA.onEnd([] { show_center_msg("OTA Sccess, Restarting..."); });
  ArduinoOTA.onError([](ota_error_t err) {
    sprintf(buf, "OTA Error [%d]!!", err);
    show_center_msg(buf);
  });
  ArduinoOTA.begin();
}

#endif