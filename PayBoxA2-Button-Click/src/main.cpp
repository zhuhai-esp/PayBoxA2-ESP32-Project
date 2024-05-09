#include "OneButton.h"
#include "lgfx.h"
#include <Arduino.h>
#include <ArduinoOTA.h>

long check1s = 0;
char buf[128] = {0};
LGFX tft;

OneButton keyOn(PAY_KEY_ON, true);
OneButton keyAdd(PAY_KEY_ADD, true);
OneButton keyMinus(PAY_KEY_MINUS, true);
OneButton keyRefresh(PAY_KEY_REFRESH, true);

inline void show_center_msg(const char *msg) {
  tft.drawCenterString(msg, TFT_WIDTH / 2, TFT_HEIGHT / 2,
                       &fonts::AsciiFont8x16);
}

void inline autoConfigWifi() {
  WiFi.mode(WIFI_MODE_STA);
  WiFi.begin();
  for (int i = 0; WiFi.status() != WL_CONNECTED && i < 100; i++) {
    delay(100);
  }
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.mode(WIFI_MODE_APSTA);
    show_center_msg("Config WiFi with ESPTouch App!");
    WiFi.beginSmartConfig();
    while (WiFi.status() != WL_CONNECTED) {
      delay(100);
    }
    WiFi.stopSmartConfig();
    WiFi.mode(WIFI_MODE_STA);
  }
}

void inline startConfigTime() {
  const int timeZone = 8 * 3600;
  configTime(timeZone, 0, "ntp6.aliyun.com", "cn.ntp.org.cn", "ntp.ntsc.ac.cn");
  while (time(nullptr) < 8 * 3600 * 2) {
    delay(500);
  }
}

void inline setupOTAConfig() {
  ArduinoOTA.onStart([] {
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawCentreString("OTA Update", 160, 60, 4);
    tft.drawRoundRect(60, 160, 200, 20, 6, TFT_ORANGE);
  });
  ArduinoOTA.onProgress([](u32_t pro, u32_t total) {
    tft.setTextColor(TFT_SKYBLUE, TFT_BLACK);
    sprintf(buf, "%d / %d", pro, total);
    tft.drawCentreString(buf, 160, 120, 2);
    int pros = pro * 200 / total;
    tft.fillRoundRect(60, 160, pros, 20, 6, TFT_ORANGE);
  });
  ArduinoOTA.onEnd([] {
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawCentreString("Update Succeed!!", 160, 60, 4);
    tft.setTextColor(TFT_SKYBLUE, TFT_BLACK);
    tft.drawCentreString("Restarting...", 160, 140, 2);
  });
  ArduinoOTA.begin();
}

void inline setupButtons() {
  keyOn.attachClick([]() {
    tft.clear();
    show_center_msg("On Click");
  });
  keyOn.attachDoubleClick([]() {
    tft.clear();
    show_center_msg("On Double Click");
  });
}

void setup() {
  Serial.begin(115200);
  setupButtons();
  tft.init();
  tft.initDMA();
  tft.startWrite();
  tft.setBrightness(50);
  show_center_msg("Start WiFi Connect!");
  tft.clear();
  autoConfigWifi();
  show_center_msg("WiFi Connected, Please Wait...");
  startConfigTime();
  tft.clear();
  sprintf(buf, "IP: %s", WiFi.localIP().toString().c_str());
  show_center_msg(buf);
  setupOTAConfig();
}

void loop() {
  auto ms = millis();
  if (ms - check1s > 1000) {
    check1s = ms;
    ArduinoOTA.handle();
  }
  keyOn.tick();
}