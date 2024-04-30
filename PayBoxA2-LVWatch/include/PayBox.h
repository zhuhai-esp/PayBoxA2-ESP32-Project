#ifndef __PAY_BOX_H__
#define __PAY_BOX_H__

#include <Arduino.h>
#include <ArduinoOTA.h>
#include <TFT_eSPI.h>

// 按钮输入
#define PAY_KEY_RESET GPIO_NUM_5
#define PAY_KEY_ON GPIO_NUM_34
#define PAY_KEY_ADD GPIO_NUM_39
#define PAY_KEY_MINUS GPIO_NUM_36
#define PAY_KEY_REFRESH GPIO_NUM_35

// LED输出
#define PAY_LED_RED GPIO_NUM_4
#define PAY_LED_GREEN GPIO_NUM_33
#define PAY_LED_BLUE GPIO_NUM_13

// 音频数字输出
#define PAY_AUDIO_SDATA GPIO_NUM_14
#define PAY_AUDIO_SCLK GPIO_NUM_26
#define PAY_AUDIO_LRCK GPIO_NUM_27
#define PAY_AUDIO_CLKIN GPIO_NUM_0
#define PAY_AUDIO_CTRL GPIO_NUM_2

// TFT输出
#define PAY_TFT_BAK GPIO_NUM_15
#define PAY_TFT_CS GPIO_NUM_22
#define PAY_TFT_MOSI GPIO_NUM_23
#define PAY_TFT_SCLK GPIO_NUM_19
#define PAY_TFT_DC GPIO_NUM_21
#define PAY_TFT_RST GPIO_NUM_18
#define PAY_TFT_MISO GPIO_NUM_25

#define TZ 8            // 中国时区为8
#define DST_MN 0        // 默认为0
#define TZ_MN ((TZ)*60) //时间换算
#define TZ_SEC ((TZ)*3600)
#define DST_SEC ((DST_MN)*60)

extern TFT_eSPI tft;
extern char buf[];

void inline payTFTInit(uint32_t brightness) {
  tft.init();
  tft.setRotation(7);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextFont(2);
  tft.setCursor(0, 0);
  ledcSetup(0, 2000, 8);
  ledcAttachPin(PAY_TFT_BAK, 0);
  ledcWrite(0, (255 - brightness));
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

// 通过AutoConfig配置Wifi热点
void inline autoConfigWifi() {
  tft.println("Start Wifi Connect!");
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
  tft.println("Wifi Connected!");
  sprintf(buf, "IP: %s", WiFi.localIP().toString().c_str());
  tft.println(buf);
}

void inline bindKeyEvent(int pin, void (*intRoutine)()) {
  pinMode(pin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pin), intRoutine, FALLING);
}

#endif
