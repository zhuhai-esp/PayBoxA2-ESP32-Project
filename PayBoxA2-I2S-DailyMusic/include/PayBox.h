#ifndef __PAY_BOX_H__
#define __PAY_BOX_H__

#include "lgfx.h"
#include <Arduino.h>
#include <ArduinoOTA.h>
#include <Audio.h>
#include <OneButton.h>
#include <map>

#define FONT16 &fonts::efontCN_16

static const char *WEEK_DAYS[] = {"日", "一", "二", "三", "四", "五", "六"};
extern char buf[];
extern LGFX tft;
extern LGFX_Sprite sp;
extern Audio audio;
extern std::map<u32_t, OneButton *> buttons;
extern void onButtonClick(void *p);
extern void onButtonDoubleClick(void *p);

void inline setupPayLED() {
  ledcSetup(1, 200, 8);
  ledcSetup(2, 200, 8);
  ledcSetup(3, 200, 8);
  ledcAttachPin(PAY_LED_RED, 1);
  ledcAttachPin(PAY_LED_GREEN, 2);
  ledcAttachPin(PAY_LED_BLUE, 3);
}

void inline ledShowH(uint32_t hi) {
  float h = hi / 360.0f;
  uint32_t r, g, b;
  if (h < 1.0f / 6.0f) {
    r = 255;
    g = 6 * h * 255;
    b = 0;
  } else if (h < 1 / 2.f) {
    r = 255 - 6 * (h - 1 / 6.f) * 255;
    g = 255;
    b = 0;
  } else if (h < 2 / 3.f) {
    r = 0;
    g = 255;
    b = 6 * (h - 1 / 2.f) * 255;
  } else {
    r = 0;
    g = 6 * (2 / 3.f - h) * 255;
    b = 255;
  }
  ledcWrite(1, r);
  ledcWrite(2, g);
  ledcWrite(3, b);
}

void ledShowColor(uint32_t color) {
  ledcWrite(1, (color & 0xff0000) >> 16);
  ledcWrite(2, (color & 0x00ff00) >> 8);
  ledcWrite(3, (color & 0x0000ff));
}

void inline setupButtons() {
  u32_t btnPins[] = {PAY_KEY_ON, PAY_KEY_ADD, PAY_KEY_MINUS, PAY_KEY_REFRESH};
  for (auto pin : btnPins) {
    auto *btn = new OneButton(pin);
    btn->attachClick(onButtonClick, (void *)pin);
    btn->attachDoubleClick(onButtonDoubleClick, (void *)pin);
    buttons.insert({pin, btn});
  }
}

void inline setupAudoPlayer() {
  pinMode(PAY_AUDIO_CTRL, OUTPUT);
  audio.setPinout(PAY_AUDIO_SCLK, PAY_AUDIO_LRCK, PAY_AUDIO_SDATA,
                  PAY_AUDIO_CLKIN);
}

inline void showClientIP() {
  sprintf(buf, "IP: %s", WiFi.localIP().toString().c_str());
  sp.createSprite(160, 16);
  sp.drawRightString(buf, 150, 0);
  sp.pushSprite(160, 220);
  sp.deleteSprite();
}

void inline autoConfigWifi() {
  tft.println("Start WiFi Connect!");
  WiFi.mode(WIFI_MODE_STA);
  WiFi.begin();
  for (int i = 0; WiFi.status() != WL_CONNECTED && i < 100; i++) {
    delay(100);
  }
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.mode(WIFI_MODE_APSTA);
    WiFi.beginSmartConfig();
    tft.println("Config WiFi with ESPTouch App!");
    while (WiFi.status() != WL_CONNECTED) {
      delay(100);
    }
    WiFi.stopSmartConfig();
    WiFi.mode(WIFI_MODE_STA);
  }
  tft.println("WiFi Connected, Please Wait...");
}

inline void showCurrentTime() {
  struct tm info;
  getLocalTime(&info);
  sprintf(buf, "%d年%d月%d日 星期%s", 1900 + info.tm_year, info.tm_mon + 1,
          info.tm_mday, WEEK_DAYS[info.tm_wday]);
  sp.createSprite(320, 16);
  sp.drawCentreString(buf, 160, 0);
  sp.pushSprite(0, 140);
  sp.deleteSprite();
  strftime(buf, 36, "%T", &info);
  sp.createSprite(320, 36);
  sp.drawCentreString(buf, 160, 0, &fonts::FreeSans24pt7b);
  sp.pushSprite(0, 170);
  sp.deleteSprite();
}

void inline startConfigTime() {
  const int timeZone = 8 * 3600;
  configTime(timeZone, 0, "ntp6.aliyun.com", "cn.ntp.org.cn", "ntp.ntsc.ac.cn");
  while (time(nullptr) < 8 * 3600 * 2) {
    delay(300);
  }
}

void inline setupOTAConfig() {
  ArduinoOTA.onStart([] {
    digitalWrite(PAY_AUDIO_CTRL, LOW);
    audio.stopSong();
    tft.setBrightness(200);
    tft.clear();
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawCentreString("OTA Update", 160, 60, &fonts::Font4);
    tft.drawRoundRect(58, 158, 204, 10, 3, TFT_ORANGE);
    tft.drawCentreString("正在升级中，请勿断电", 160, 190, FONT16);
  });
  ArduinoOTA.onProgress([](u32_t pro, u32_t total) {
    sprintf(buf, "升级进度: %d / %d", pro, total);
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.drawCentreString(buf, 160, 120, FONT16);
    if (pro > 0 && total > 0) {
      int pros = pro * 200 / total;
      tft.fillRoundRect(60, 160, pros, 6, 2, TFT_WHITE);
    }
  });
  ArduinoOTA.onEnd([] {
    tft.clear();
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawCentreString("Update Succeed!!", 160, 60, &fonts::Font4);
    tft.drawCentreString("升级成功，正在重启...", 160, 140, FONT16);
  });
  ArduinoOTA.onError([](ota_error_t e) {
    tft.clear();
    ESP.restart();
  });
  ArduinoOTA.begin();
  sprintf(buf, "%s", WiFi.localIP().toString().c_str());
  tft.println(buf);
  struct tm info;
  getLocalTime(&info);
  strftime(buf, 64, "%c", &info);
  tft.println(buf);
}

#endif
