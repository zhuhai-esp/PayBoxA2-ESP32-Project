#ifndef __WT32_LV_H__
#define __WT32_LV_H__

#define LGFX_USE_V1

#include "lgfx.h"
#include <Arduino.h>
#include <lvgl.h>

LGFX tft;

#define LV_DISP_HOR_RES TFT_WIDTH
#define LV_DISP_VER_RES TFT_HEIGHT

static const uint32_t buf_size = LV_DISP_HOR_RES * 10;
static lv_color_t dis_buf1[buf_size];
// static lv_color_t dis_buf2[buf_size];

void inline lv_disp_init() {
  auto *disp = lv_display_create(LV_DISP_HOR_RES, LV_DISP_VER_RES);
  auto f_disp = [](lv_display_t *disp, const lv_area_t *area, uint8_t *px_map) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);
    auto dat = (lgfx::rgb565_t *)px_map;
    tft.setAddrWindow(area->x1, area->y1, w, h);
    tft.pushPixelsDMA(dat, w * h);
    // tft.pushImageDMA(area->x1, area->y1, w, h, dat);
    lv_display_flush_ready(disp);
  };
  lv_display_set_flush_cb(disp, f_disp);
  auto mode = LV_DISPLAY_RENDER_MODE_PARTIAL;
  lv_display_set_buffers(disp, dis_buf1, nullptr, buf_size, mode);
}

inline void init_wt32_lvgl() {
  tft.init();
  tft.initDMA();
  tft.startWrite();
  tft.setBrightness(50);

  lv_init();
  lv_disp_init();
}

inline void show_center_msg(const char *msg) {
  tft.drawCenterString(msg, TFT_WIDTH / 2, TFT_HEIGHT / 2,
                       &fonts::AsciiFont8x16);
}

#endif