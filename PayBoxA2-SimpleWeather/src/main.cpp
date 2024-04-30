#include "web_api.h"
#include "wt32_lv.h"
#include "wt32_wifi.h"
#include <Arduino.h>

using namespace std;

LV_FONT_DECLARE(simsun_16)
LV_FONT_DECLARE(led_48)

static const char *WEEK_DAYS[] = {"日", "一", "二", "三", "四", "五", "六"};

long check1s = 0;
long check5ms = 0;
long check5min = 0;

lv_obj_t *label_date;
lv_obj_t *label_time;
lv_obj_t *label_loc;
lv_obj_t *label_wind;
lv_obj_t *label_ip;

static json city_info;
static json weather;

void start_lv_app() {
  lv_obj_t *parent = lv_scr_act();
  label_date = lv_label_create(parent);
  lv_label_set_text(label_date, "2000年01月01日 星期一");
  lv_obj_set_style_text_font(label_date, &simsun_16, LV_PART_MAIN);
  lv_obj_align(label_date, LV_ALIGN_TOP_MID, 0, 10);

  label_time = lv_label_create(parent);
  lv_label_set_text(label_time, "08:00:00");
  lv_obj_set_style_text_font(label_time, &led_48, LV_PART_MAIN);
  lv_obj_align(label_time, LV_ALIGN_TOP_MID, 0, 45);

  label_loc = lv_label_create(parent);
  lv_label_set_text(label_loc, "");
  lv_obj_set_style_text_font(label_loc, &simsun_16, LV_PART_MAIN);
  lv_obj_align(label_loc, LV_ALIGN_TOP_MID, 0, 100);

  label_wind = lv_label_create(parent);
  lv_label_set_text(label_wind, "");
  lv_obj_set_style_text_font(label_wind, &simsun_16, LV_PART_MAIN);
  lv_obj_align(label_wind, LV_ALIGN_TOP_MID, 0, 130);

  label_ip = lv_label_create(parent);
  lv_label_set_text(label_ip, "IP: 127.0.0.1");
  lv_obj_align(label_ip, LV_ALIGN_BOTTOM_MID, 0, -10);
}

inline void showCurrentTime() {
  struct tm info;
  getLocalTime(&info);
  strftime(buf, 32, "%T", &info);
  lv_label_set_text(label_time, buf);
  sprintf(buf, "%d年%d月%d日 星期%s", 1900 + info.tm_year, info.tm_mon + 1,
          info.tm_mday, WEEK_DAYS[info.tm_wday]);
  lv_label_set_text(label_date, buf);
}

inline void showClientIP() {
  sprintf(buf, "IP: %s", WiFi.localIP().toString().c_str());
  lv_label_set_text(label_ip, buf);
}

inline void updateWeatherInfo() {
  weather = get_now_weather_info(city_info["cityId"]);
  json ip_info = city_info["ip"];
  string province = ip_info["province"], city = ip_info["city"],
         district = ip_info["district"], condition = weather["CONDITIONSTEXT"],
         wind = weather["WIND"], aiq_str = weather["levelIndex"];
  int aqi = weather["aqi"];
  int temp = weather["TEMP"], winp = weather["WINP"], humi = weather["HUMI"];
  sprintf(buf, "%s %s %s %s %d℃", province.c_str(), city.c_str(),
          district.c_str(), condition.c_str(), temp);
  lv_label_set_text(label_loc, buf);
  sprintf(buf, "%s %d级 | 湿度 %d%% | 空气 %d %s", wind.c_str(), winp, humi,
          aqi, aiq_str.c_str());
  lv_label_set_text(label_wind, buf);
}

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
  show_center_msg("Start WiFi Connect!");
  autoConfigWifi();
  tft.clear();
  show_center_msg("WiFi Connected, Please Wait...");
  startConfigTime();
  setupOTAConfig();
  start_lv_app();
  showClientIP();
  city_info = get_cur_city_info();
  updateWeatherInfo();
}

void loop() {
  auto ms = millis();
  if (ms - check1s > 1000) {
    check1s = ms;
    ArduinoOTA.handle();
    showCurrentTime();
  }
  if (ms - check5ms > 5) {
    check5ms = ms;
    lv_timer_handler();
  }
  if (ms - check5min > 5 * 60 * 1000) {
    check5min = ms;
    updateWeatherInfo();
  }
}