#ifndef __WT32_LV_H__
#define __WT32_LV_H__

#define LGFX_USE_V1

#include "lgfx.h"
#include <Arduino.h>
#include <lvgl.h>

LGFX tft;

static lv_disp_draw_buf_t draw_buf;
static lv_disp_drv_t disp_drv;

static const int buf_size = TFT_WIDTH * 10;
static lv_color_t disp_draw_buf[buf_size];
static lv_color_t disp_draw_buf2[buf_size];


void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area,
                   lv_color_t *color_p) {
  tft.pushImageDMA(area->x1, area->y1, area->x2 - area->x1 + 1,
                   area->y2 - area->y1 + 1, (lgfx::rgb565_t *)&color_p->full);
  lv_disp_flush_ready(disp); 
}

inline void init_wt32_lvgl() {
  tft.init();
  tft.initDMA();
  tft.startWrite();
  tft.setBrightness(50);

  lv_init();
  lv_disp_draw_buf_init(&draw_buf, disp_draw_buf, disp_draw_buf2, buf_size);
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = TFT_WIDTH;
  disp_drv.ver_res = TFT_HEIGHT;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);
}

inline void show_center_msg(const char *msg) {
  tft.drawCenterString(msg, TFT_WIDTH / 2, TFT_HEIGHT / 2,
                       &fonts::AsciiFont8x16);
}

#endif