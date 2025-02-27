#include "BluetoothA2DPSink.h"
#include "OneButton.h"
#include "lgfx.h"
#include <Arduino.h>
#include <ArduinoOTA.h>

#define FONT16 &fonts::efontCN_16

static const char *WEEK_DAYS[] = {"日", "一", "二", "三", "四", "五", "六"};
BluetoothA2DPSink a2dp_sink;
long check1s = 0, check10ms = 0, check300ms = 0;
long checkDark = 0;
char buf[128] = {0};
LGFX tft;
uint8_t volume = 50;
bool isPause = false;
OneButton buttons[] = {OneButton(PAY_KEY_ON), OneButton(PAY_KEY_ADD),
                       OneButton(PAY_KEY_MINUS), OneButton(PAY_KEY_REFRESH)};

void show_center_msg(const char *msg) {
  tft.drawCenterString(msg, TFT_WIDTH / 2, TFT_HEIGHT / 2, FONT16);
}

void payAudioDisable() {
  pinMode(PAY_AUDIO_CTRL, OUTPUT);
  digitalWrite(PAY_AUDIO_CTRL, LOW);
}

void payAudioEnable() {
  pinMode(PAY_AUDIO_CTRL, OUTPUT);
  digitalWrite(PAY_AUDIO_CTRL, HIGH);
}

void inline autoConfigWifi() {
  WiFi.mode(WIFI_MODE_STA);
  WiFi.begin();
  for (int i = 0; WiFi.status() != WL_CONNECTED && i < 100; i++) {
    delay(100);
  }
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.mode(WIFI_MODE_APSTA);
    tft.clear();
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
    a2dp_sink.pause();
    payAudioDisable();
    tft.clear();
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawCentreString("OTA Update", 160, 60, FONT16);
    tft.drawRoundRect(60, 160, 200, 20, 4, TFT_ORANGE);
  });
  ArduinoOTA.onProgress([](u32_t pro, u32_t total) {
    tft.setTextColor(TFT_SKYBLUE, TFT_BLACK);
    sprintf(buf, "%d / %d", pro, total);
    tft.drawCentreString(buf, 160, 120, FONT16);
    int pros = pro * 200 / total;
    tft.fillRoundRect(60, 160, pros, 20, 4, TFT_ORANGE);
  });
  ArduinoOTA.onEnd([] {
    tft.clear();
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawCentreString("Update Succeed!!", 160, 60, FONT16);
    tft.setTextColor(TFT_SKYBLUE, TFT_BLACK);
    tft.drawCentreString("Restarting...", 160, 140, FONT16);
  });
  ArduinoOTA.begin();
}

void inline setVolume(uint8_t v) {
  volume = v;
  a2dp_sink.set_volume(volume);
  sprintf(buf, "Volume: %02d", volume / 10);
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.drawCentreString(buf, 160, 160, FONT16);
  tft.fillRoundRect(60, 140, 200, 8, 2, TFT_BLACK);
  tft.drawRoundRect(60, 140, 200, 8, 2, TFT_PINK);
  int pros = v * 200 / 120;
  tft.fillRoundRect(60, 140, pros, 8, 2, TFT_PINK);
}

void payAudioInit() {
  PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0_CLK_OUT1);
  REG_WRITE(PIN_CTRL, 0xFFFFF0);
  pinMode(PAY_AUDIO_CTRL, OUTPUT);
  digitalWrite(PAY_AUDIO_CTRL, HIGH);
}

void inline setupAudioPlay() {
  setVolume(50);
  const i2s_pin_config_t pc = {.bck_io_num = PAY_AUDIO_SCLK,
                               .ws_io_num = PAY_AUDIO_LRCK,
                               .data_out_num = PAY_AUDIO_SDATA,
                               .data_in_num = I2S_PIN_NO_CHANGE};
  a2dp_sink.set_pin_config(pc);
  a2dp_sink.start("PayBox A2", true);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.drawCentreString("蓝牙播放器", 160, 90, &fonts::efontCN_24);
  payAudioInit();
}

void inline setupButtonEvents() {
  buttons[0].attachClick([] {
    tft.setBrightness(60);
    checkDark = 0;
    payAudioDisable();
    ESP.restart();
  });
  buttons[1].attachClick([] {
    tft.setBrightness(60);
    checkDark = 0;
    a2dp_sink.next();
  });
  buttons[1].attachDoubleClick([] {
    if (volume < 120) {
      setVolume(volume + 10);
    }
  });
  buttons[2].attachClick([] {
    tft.setBrightness(60);
    checkDark = 0;
    a2dp_sink.previous();
  });
  buttons[2].attachDoubleClick([] {
    if (volume > 10) {
      setVolume(volume - 10);
    }
  });
  buttons[3].attachClick([] {
    tft.setBrightness(60);
    checkDark = 0;
    if (isPause) {
      a2dp_sink.play();
      payAudioEnable();
    } else {
      payAudioDisable();
      a2dp_sink.pause();
    }
    isPause = !isPause;
  });
}

void setup() {
  Serial.begin(115200);
  tft.init();
  tft.setBrightness(60);
  payLEDInit();
  payLEDShow(0xf6229a);
  show_center_msg("Start WiFi Connect!");
  autoConfigWifi();
  tft.clear();
  show_center_msg("WiFi Connected, Please Wait...");
  startConfigTime();
  tft.clear();
  sprintf(buf, "IP地址: %s", WiFi.localIP().toString().c_str());
  tft.drawCenterString(buf, 160, 220, FONT16);
  setupOTAConfig();
  setupAudioPlay();
  setupButtonEvents();
}

void inline showCurrentTime() {
  struct tm info;
  getLocalTime(&info);
  tft.setTextColor(TFT_OLIVE, TFT_BLACK);
  sprintf(buf, "%d年%d月%d日 星期%s", 1900 + info.tm_year, info.tm_mon + 1,
          info.tm_mday, WEEK_DAYS[info.tm_wday]);
  tft.drawCenterString(buf, 160, 8, &fonts::efontCN_24);
  strftime(buf, 32, "%T", &info);
  tft.setTextColor(TFT_DARKCYAN, TFT_BLACK);
  tft.drawCenterString(buf, 160, 46, &fonts::FreeSerif18pt7b);
}

void loop() {
  auto ms = millis();
  if (ms - check1s > 1000) {
    check1s = ms;
    auto led = random(0x050505, 0xffffff);
    payLEDShow(led);
    ArduinoOTA.handle();
    checkDark += 1;
    if (checkDark == 30) {
      tft.setBrightness(16);
    }
  }
  if (ms - check300ms >= 300) {
    check300ms = ms;
    showCurrentTime();
  }
  if (ms - check10ms >= 10) {
    check10ms = ms;
    for (int i = 0; i < 4; i++) {
      buttons[i].tick();
    }
  }
}