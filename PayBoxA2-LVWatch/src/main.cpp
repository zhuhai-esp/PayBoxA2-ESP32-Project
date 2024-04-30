#include "wt32_lv.h"
#include "wt32_wifi.h"
#include <Arduino.h>

long check1s = 0;
long check5ms = 0;

lv_obj_t *label_date;
lv_obj_t *label_ip;
lv_obj_t *label_time;
lv_obj_t *img_second;
lv_obj_t *img_minute;
lv_obj_t *img_hour;

LV_IMG_DECLARE(watch_bg);
LV_IMG_DECLARE(hour);
LV_IMG_DECLARE(minute);
LV_IMG_DECLARE(second);

void ui_init(void) {
  auto *parent = lv_scr_act();

  label_date = lv_label_create(parent);
  lv_obj_align(label_date, LV_ALIGN_TOP_RIGHT, -4, 4);
  lv_label_set_text(label_date, "2000-01-01");

  label_ip = lv_label_create(parent);
  lv_obj_align(label_ip, LV_ALIGN_BOTTOM_RIGHT, -4, -4);
  lv_obj_set_style_text_font(label_ip, &lv_font_montserrat_12, LV_PART_MAIN);
  lv_label_set_text(label_ip, "IP: 0.0.0.0");

  label_time = lv_label_create(parent);
  lv_obj_align(label_time, LV_ALIGN_BOTTOM_LEFT, 4, -4);
  lv_label_set_text(label_time, "12:33:45");

  auto *panel = lv_img_create(parent);
  lv_img_set_src(panel, &watch_bg);
  lv_obj_set_align(panel, LV_ALIGN_CENTER);

  img_hour = lv_img_create(parent);
  lv_img_set_src(img_hour, &hour);
  lv_obj_set_align(img_hour, LV_ALIGN_CENTER);

  img_minute = lv_img_create(parent);
  lv_img_set_src(img_minute, &minute);
  lv_obj_set_align(img_minute, LV_ALIGN_CENTER);

  img_second = lv_img_create(parent);
  lv_img_set_src(img_second, &second);
  lv_obj_set_align(img_second, LV_ALIGN_CENTER);
}

inline void showClientIP() {
  sprintf(buf, "IP: %s", WiFi.localIP().toString().c_str());
  lv_label_set_text(label_ip, buf);
}

inline void showCurrentTime() {
  struct tm info;
  getLocalTime(&info);
  strftime(buf, 32, "%F", &info);
  lv_label_set_text(label_date, buf);
  strftime(buf, 32, "%T", &info);
  lv_label_set_text(label_time, buf);

  lv_img_set_angle(img_hour, info.tm_hour * 300 + info.tm_min / 12 % 12 * 60);
  lv_img_set_angle(img_minute, info.tm_min * 6 * 10);
  lv_img_set_angle(img_second, info.tm_sec * 6 * 10);
}

void setup() {
  Serial.begin(115200);
  init_wt32_lvgl();
  ui_init();
  show_center_msg("Please Config WiFi With ESP-Touch!");
  autoConfigWifi();
  configTime(TZ_SEC, DST_SEC, "ntp.ntsc.ac.cn", "ntp1.aliyun.com");
  setupOTAConfig();
  showClientIP();
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
}