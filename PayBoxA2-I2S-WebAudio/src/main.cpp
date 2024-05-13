#include "lgfx.h"
#include <Arduino.h>
#include <ArduinoOTA.h>
#include <Audio.h>
#include <OneButton.h>
#include <map>
#include <vector>

#define FONT16 &fonts::efontCN_16
#define FM_URL "http://lhttp.qtfm.cn/live/%d/64k.mp3"

typedef struct {
  u32_t id;
  String name;
} RadioItem;

static const char *WEEK_DAYS[] = {"日", "一", "二", "三", "四", "五", "六"};
long check1s = 0, check10ms = 0, check300ms = 0;
char buf[128] = {0};
LGFX tft;
LGFX_Sprite sp(&tft);
Audio audio;
int curIndex = 0;
int curVolume = 5;
std::map<u32_t, OneButton *> buttons;
std::vector<RadioItem> radios = {
    {20500149, "两广之声音乐台"},
    {4804, "怀集音乐之声"},
    {1649, "河北音乐广播"},
    {4915, "清晨音乐台"},
    {4938, "江苏经典流行音乐"},
    {1260, "广东音乐之声"},
    {273, "上海流行音乐LoveRadio"},
    {1223, "怀旧好声音"},
    {274, "上海动感101"},
    {2803, "苏州音乐广播"},
    {839, "哈尔滨音乐广播"},
    {5022107, "动听音乐台"},
    {5021381, "959年代音乐怀旧好声音"},
    {15318569, "AsiaFM 亚洲粤语台"},
    {5022308, "500首华语经典"},
    {20500150, "顺德音乐之声"},
    {4875, "FM950广西音乐台"},
    {1283, "江门旅游之声"},
    {1936, "FM954汽车音乐广播"},
    {332, "北京音乐广播"},
    {4932, "山西音乐广播"},
    {4866, "浙江音乐调频"},
    {20211686, "成都年代音乐怀旧好声音"},
    {1739, "厦门音乐广播"},
    {20462, "经典调频北京FM969"},
    {1271, "深圳飞扬971"},
    {20240, "山东经典音乐广播"},
    {20500066, "年代音乐1022"},
    {2779, "无锡音乐广播"},
    {1296, "湖北经典音乐广播"},
    {1206, "流行音乐先锋·90My Radio"},
    {267, "上海经典947"},
    {20212426, "崂山921"},
    {20003, "天津TIKI FM100.5"},
    {1111, "四川城市之音"},
    {4936, "江苏音乐广播PlayFM897"},
    {4237, "长沙FM101.7城市之声"},
    {1665, "山东音乐广播"},
    {470, "广东广播电视台珠江之声"},
    {4850, "长春广播电视台 FM88.0"},
    {1947, "安徽音乐广播"},
    {5021743, "汽车音乐台"},
    {20847, "FM88.6长沙音乐广播"},
    {1612, "西安音乐广播"},
    {20210755, "星河音乐"},
    {1886, "内蒙古音乐之声"},
    {1208, "河南音乐广播"},
    {4963, "南京音乐广播"},
    {1802, "江西音乐广播"},
    {15318146, "杭州FM90.7"},
    {647, "重庆音乐广播"},
    {15318703, "欧美音乐88.7"},
    {5021523, "惠州音乐广播"},
    {15318341, "AsiaFM HD音乐台"},
    {20769, "南宁经典1049"},
    {1289, "楚天音乐广播"},
    {4873, "陕西音乐广播"},
    {5022474, "武安融媒综合广播"},
    {21209, "东莞音乐广播"},
    {4969, "黑龙江音乐广播"},
    {1136, "嘉兴音乐广播"},
    {21275, "南通音乐广播"},
    {20211619, "怀旧音乐广播895"},
    {4981, "芒果时空音乐台"},
    {1297, "武汉经典音乐广播"},
    {20211638, "定州交通音乐广播"},
    {5022023, "上海KFM981"},
    {20207761, "80后音悦台"},
    {1654, "石家庄音乐广播"},
    {20212227, "经典FM1008"},
    {1149, "1003温州音乐之声"},
    {1671, "济南音乐广播FM88.7"},
    {5021912, "AsiaFM 亚洲经典台"},
    {1084, "大连1067"},
    {1892, "包头汽车音乐广播"},
    {1110, "四川岷江音乐广播"},
    {1831, "吉林音乐广播"},
    {5022405, "AsiaFM 亚洲音乐台"},
    {4581, "亚洲音乐成都FM96.5"},
    {20071, "AsiaFM 亚洲天空台"},
    {20033, "1047 Nice FM"},
    {4930, "FM102.2亲子智慧电台"},
    {4846, "893音乐广播"},
    {20026, "郁南音乐台"},
    {1608, "陕西故事广播·年代878"},
    {4923, "徐州音乐广播FM91.9"},
    {4878, "海南音乐广播"},
    {20211575, "经典983电台"},
    {4594, "潮州交通音乐广播"},
    {20500097, "经典音乐广播FM94.8"},
    {4885, "陕西青少广播·好听1055"},
    {4585, "福建音乐广播"},
    {2799, "常州音乐广播"},
    {1975, "MUSIC876"},
    {5022391, "Easy Fm"},
    {20500067, "FM95.9清远交通音乐广播"},
    {20211620, "流行音乐广播999正青春"},
    {20067, "贵州FM91.6音乐广播"},
    {5021902, "沧州音乐广播FM103.6"},
    {20207781, "眉山交通音乐广播"},
    {2811, "湖州交通文艺广播"},
    {5022050, "FM89.1吴江综合广播"},
    {20500053, "经典958"},
    {5022520, "盛京FM105.6"},
    {20091, "中国校园之声"},
    {4979, "89.3芒果音乐台"},
    {20835, "秦皇岛音乐广播"},
    {20211678, "廊坊飞扬105"},
    {1677, "青岛音乐体育广播"},
    {4029, "新疆MIXFM1039"},
    {5022338, "冰城1026哈尔滨古典音乐广播"},
    {20207762, "河南经典FM"},
    {4921, "郑州音乐广播"},
    {5022610, "察布查尔FM99.5"},
    {4871, "唐山音乐广播"},
    {1683, "烟台音乐广播FM105.9"},
    {5020, "滁州旅游交通广播"},
    {20440, "新疆昌吉 FM103.3综合广播"},
    {20212387, "凤凰音乐"},
    {20500187, "云梦音乐台"},
};

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
    tft.clear();
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawCentreString("OTA Update", 160, 60, &fonts::Font4);
    tft.drawRoundRect(58, 158, 204, 20, 4, TFT_ORANGE);
  });
  ArduinoOTA.onProgress([](u32_t pro, u32_t total) {
    tft.setTextColor(TFT_SKYBLUE, TFT_BLACK);
    sprintf(buf, "%d / %d", pro, total);
    tft.drawCentreString(buf, 160, 120, FONT16);
    int pros = pro * 200 / total;
    tft.fillRoundRect(60, 160, pros, 16, 4, TFT_GREEN);
  });
  ArduinoOTA.onEnd([] {
    tft.clear();
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawCentreString("Update Succeed!!", 160, 60, &fonts::Font4);
    tft.setTextColor(TFT_SKYBLUE, TFT_BLACK);
    tft.drawCentreString("Restarting...", 160, 140, FONT16);
  });
  ArduinoOTA.begin();
  sprintf(buf, "IP: %s", WiFi.localIP().toString().c_str());
  tft.println(buf);
  struct tm info;
  getLocalTime(&info);
  strftime(buf, 64, "%c", &info);
  tft.println(buf);
}

void nextVolume(int offset) {
  int vol = curVolume + offset;
  if (vol >= 0 && vol <= 21) {
    curVolume = vol;
    audio.setVolume(curVolume);
    sprintf(buf, "音量: %d", curVolume);
    sp.createSprite(320, 16);
    sp.drawCentreString(buf, 160, 0);
    sp.pushSprite(0, 50);
    sp.deleteSprite();
  }
}

void playNext(int offset) {
  int total = radios.size();
  curIndex += offset;
  if (curIndex >= total) {
    curIndex %= total;
  } else if (curIndex < 0) {
    curIndex += total;
  }
  auto radio = radios[curIndex];
  sprintf(buf, FM_URL, radio.id);
  audio.connecttohost(buf);
  sprintf(buf, "%d.%s", curIndex + 1, radio.name.c_str());
  sp.createSprite(320, 16);
  sp.drawCentreString(buf, 160, 0);
  sp.pushSprite(0, 20);
  sp.deleteSprite();
}

void onButtonClick(void *p) {
  u32_t pin = (u32_t)p;
  switch (pin) {
  case PAY_KEY_ON:
    playNext(10);
    break;
  case PAY_KEY_REFRESH:
    audio.pauseResume();
    break;
  case PAY_KEY_ADD:
    playNext(1);
    break;
  case PAY_KEY_MINUS:
    playNext(-1);
    break;
  default:
    break;
  }
}

void onButtonDoubleClick(void *p) {
  u32_t pin = (u32_t)p;
  switch (pin) {
  case PAY_KEY_ON:
    playNext(radios.size() / 2);
    break;
  case PAY_KEY_ADD:
    nextVolume(1);
    break;
  case PAY_KEY_MINUS:
    nextVolume(-1);
    break;
  default:
    break;
  }
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
  digitalWrite(PAY_AUDIO_CTRL, HIGH);
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

void setup() {
  Serial.begin(115200);
  tft.init();
  tft.setFont(FONT16);
  sp.setFont(FONT16);
  tft.setBrightness(30);
  autoConfigWifi();
  startConfigTime();
  setupOTAConfig();
  setupAudoPlayer();
  setupButtons();
  sleep(2);
  tft.clear();
  showClientIP();
  nextVolume(0);
  playNext(0);
}

inline void showCurrentTime() {
  struct tm info;
  getLocalTime(&info);

  sprintf(buf, "%d年%d月%d日 星期%s", 1900 + info.tm_year, info.tm_mon + 1,
          info.tm_mday, WEEK_DAYS[info.tm_wday]);
  sp.createSprite(320, 16);
  sp.drawCentreString(buf, 160, 0);
  sp.pushSprite(0, 90);
  sp.deleteSprite();

  strftime(buf, 36, "%T", &info);
  sp.createSprite(320, 36);
  sp.drawCentreString(buf, 160, 0, &fonts::FreeSans24pt7b);
  sp.pushSprite(0, 120);
  sp.deleteSprite();
}

void loop() {
  audio.loop();
  auto ms = millis();
  if (ms - check1s > 1000) {
    check1s = ms;
    ArduinoOTA.handle();
  }
  if (ms - check300ms > 300) {
    check300ms = ms;
    showCurrentTime();
  }
  if (ms - check10ms >= 10) {
    check10ms = ms;
    for (auto it : buttons) {
      it.second->tick();
    }
  }
}
