#ifndef __WT32_LV_H__
#define __WT32_LV_H__

#define LGFX_USE_V1

#include "lgfx.h"
#include <Arduino.h>
#include <lvgl.h>

// Create an instance of the prepared class.
LGFX tft;

static lv_disp_draw_buf_t draw_buf;
static lv_disp_drv_t disp_drv;

static lv_color_t disp_draw_buf[screenWidth * SCR];
static lv_color_t disp_draw_buf2[screenWidth * SCR];

/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area,
                   lv_color_t *color_p) {
  if (tft.getStartCount() == 0) {
    tft.endWrite();
  }
  tft.pushImageDMA(area->x1, area->y1, area->x2 - area->x1 + 1,
                   area->y2 - area->y1 + 1, (lgfx::swap565_t *)&color_p->full);
  lv_disp_flush_ready(disp); /* tell lvgl that flushing is done */
}

inline void init_wt32_lvgl() {
  tft.init();
  tft.initDMA();
  tft.startWrite();
  tft.setBrightness(50);

  lv_init();
  lv_disp_draw_buf_init(&draw_buf, disp_draw_buf, disp_draw_buf2,
                        screenWidth * SCR);
  /* Initialize the display */
  lv_disp_drv_init(&disp_drv);
  /* Change the following line to your display resolution */
  disp_drv.hor_res = screenWidth;
  disp_drv.ver_res = screenHeight;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);
}

inline void show_center_msg(const char *msg) {
  tft.drawCenterString(msg, screenWidth / 2, screenHeight / 2,
                       &fonts::AsciiFont8x16);
}

#endif