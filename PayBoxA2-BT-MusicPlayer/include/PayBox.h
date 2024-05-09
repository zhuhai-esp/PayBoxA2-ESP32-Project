#ifndef __PAY_BOX_H__
#define __PAY_BOX_H__

#include <Arduino.h>

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

#define TFT_WIDTH 320
#define TFT_HEIGHT 240

void payLEDShow(uint32_t color) {
  ledcWrite(1, (color & 0xff0000) >> 16);
  ledcWrite(2, (color & 0x00ff00) >> 8);
  ledcWrite(3, (color & 0x0000ff));
}

inline void payLEDInit() {
  ledcSetup(1, 44100, 8);
  ledcSetup(2, 44100, 8);
  ledcSetup(3, 44100, 8);
  ledcAttachPin(PAY_LED_RED, 1);
  ledcAttachPin(PAY_LED_GREEN, 2);
  ledcAttachPin(PAY_LED_BLUE, 3);
}

#endif
