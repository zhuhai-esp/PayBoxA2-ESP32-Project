#include "lgfx.h"
#include <Arduino.h>
#include <ArduinoOTA.h>
#include <Audio.h>

long check1s = 0;
char buf[128] = {0};
LGFX tft;
Audio audio;

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
    WiFi.beginSmartConfig();
    show_center_msg("Config WiFi with ESPTouch App!");
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
    digitalWrite(PAY_AUDIO_CTRL, LOW);
    audio.stopSong();
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

void inline setupAudoPlayer() {
  pinMode(PAY_AUDIO_CTRL, OUTPUT);
  digitalWrite(PAY_AUDIO_CTRL, HIGH);
  audio.setPinout(PAY_AUDIO_SCLK, PAY_AUDIO_LRCK, PAY_AUDIO_SDATA,
                  PAY_AUDIO_CLKIN);
  audio.setVolume(6);
  audio.connecttohost("http://lhttp.qtfm.cn/live/5021381/64k.mp3");
}

void setup() {
  Serial.begin(115200);
  tft.init();
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
  setupAudoPlayer();
}

void loop() {
  audio.loop();
  auto ms = millis();
  if (ms - check1s > 1000) {
    check1s = ms;
    ArduinoOTA.handle();
  }
}
