#ifndef __LGFX_H__
#define __LGFX_H__

#include <LovyanGFX.hpp>
#include <PayGpio.h>

#define TFT_WIDTH 320
#define TFT_HEIGHT 240

class LGFX : public lgfx::LGFX_Device {

  lgfx::Panel_ILI9341 _panel_instance;

  lgfx::Bus_SPI _bus_instance;

  lgfx::Light_PWM _light_instance;

public:
  LGFX(void) {
    {
      // Get the structure for bus configuration.
      auto cfg = _bus_instance.config();

      // SPI bus settings
      cfg.spi_host = VSPI_HOST; // Select the SPI to use ESP32-S2,C3 : SPI2_HOST
                                // or SPI3_HOST / ESP32 : VSPI_HOST or HSPI_HOST
      // * With the ESP-IDF version upgrade, VSPI_HOST and HSPI_HOST
      // descriptions are deprecated, so if an error occurs, use SPI2_HOST and
      // SPI3_HOST instead.
      cfg.spi_mode = 3;          // Set SPI communication mode (0 ~ 3)
      cfg.freq_write = 79990000; // SPI clock when sending (up to 80MHz, rounded
                                 // to 80MHz divided by an integer)
      cfg.freq_read = 6000000;   // SPI clock when receiving
      cfg.spi_3wire = false;     // set to true if receiving on MOSI pin
      cfg.use_lock = true;       // set to true to use transaction lock
      // Set the DMA channel to use (0=not use DMA / 1=1ch
      cfg.dma_channel = SPI_DMA_CH_AUTO;
      // / 2=ch / SPI_DMA_CH_AUTO=auto setting)
      // * With the ESP-IDF version upgrade, SPI_DMA_CH_AUTO (automatic setting)
      // is recommended for the DMA channel. Specifying 1ch and 2ch is
      // deprecated.
      cfg.pin_sclk = PAY_TFT_SCLK; // set SPI SCLK pin number
      cfg.pin_mosi = PAY_TFT_MOSI; // Set MOSI pin number for SPI
      cfg.pin_miso = PAY_TFT_MISO; // set SPI MISO pin number (-1 = disable)
      cfg.pin_dc = PAY_TFT_DC;     // Set SPI D/C pin number (-1 = disable)

      _bus_instance.config(cfg);              // Apply the settings to the bus.
      _panel_instance.setBus(&_bus_instance); // Sets the bus to the panel.
    }

    { // Set display panel control.
      // Get the structure for display panel settings.
      auto cfg = _panel_instance.config();

      // Pin number to which CS is connected (-1 = disable)
      cfg.pin_cs = PAY_TFT_CS;
      // pin number where RST is connected (-1 = disable)
      cfg.pin_rst = PAY_TFT_RST;
      cfg.pin_busy = -1; // pin number to which BUSY is connected (-1 = disable)

      // * The following setting values ​​are set to general default values
      // ​​for each panel, and the pin number (-1 = disable) to which BUSY
      // is connected, so please try commenting out any unknown items.

      cfg.memory_width = TFT_WIDTH;   // Maximum width supported by driver IC
      cfg.memory_height = TFT_HEIGHT; // Maximum height supported by driver IC
      cfg.panel_width = TFT_WIDTH;    // actual displayable width
      cfg.panel_height = TFT_HEIGHT;  // actual displayable height
      cfg.offset_x = 0;               // Panel offset in X direction
      cfg.offset_y = 0;               // Panel offset in Y direction
      cfg.offset_rotation = 6;
      cfg.dummy_read_pixel = 8;
      cfg.dummy_read_bits = 1;
      cfg.readable = false;
      cfg.invert = false;
      cfg.rgb_order = true;
      cfg.dlen_16bit = false;
      cfg.bus_shared = false;

      _panel_instance.config(cfg);
    }

    { // Set backlight control. (delete if not necessary)
      // Get the structure for backlight configuration.
      auto cfg = _light_instance.config();
      // pin number to which the backlight is connected
      cfg.pin_bl = PAY_TFT_BAK;
      cfg.invert = true;   // true to invert backlight brightness
      cfg.freq = 20000;     // backlight PWM frequency
      cfg.pwm_channel = 0; // PWM channel number to use

      _light_instance.config(cfg);
      // Sets the backlight to the panel.
      _panel_instance.setLight(&_light_instance);
    }

    setPanel(&_panel_instance); // Sets the panel to use.
  }
};

#endif