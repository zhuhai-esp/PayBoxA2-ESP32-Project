#ifndef __LGFX_H__
#define __LGFX_H__

#include <LovyanGFX.hpp>

#define SCR 8

/* Change to your screen resolution */
#ifdef PORTRAIT
static const uint32_t screenWidth = 240;
static const uint32_t screenHeight = 320;
#else
static const uint32_t screenWidth = 320;
static const uint32_t screenHeight = 240;
#endif

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
      cfg.pin_sclk = 19; // set SPI SCLK pin number
      cfg.pin_mosi = 23; // Set MOSI pin number for SPI
      cfg.pin_miso = 25; // set SPI MISO pin number (-1 = disable)
      cfg.pin_dc = 21;   // Set SPI D/C pin number (-1 = disable)

      _bus_instance.config(cfg);              // Apply the settings to the bus.
      _panel_instance.setBus(&_bus_instance); // Sets the bus to the panel.
    }

    { // Set display panel control.
      // Get the structure for display panel settings.
      auto cfg = _panel_instance.config();

      cfg.pin_cs = 22;   // Pin number to which CS is connected (-1 = disable)
      cfg.pin_rst = 18;  // pin number where RST is connected (-1 = disable)
      cfg.pin_busy = -1; // pin number to which BUSY is connected (-1 = disable)

      // * The following setting values ​​are set to general default values
      // ​​for each panel, and the pin number (-1 = disable) to which BUSY
      // is connected, so please try commenting out any unknown items.

      cfg.memory_width = 320;  // Maximum width supported by driver IC
      cfg.memory_height = 240; // Maximum height supported by driver IC
      cfg.panel_width = 320;   // actual displayable width
      cfg.panel_height = 240;  // actual displayable height
      cfg.offset_x = 0;        // Panel offset in X direction
      cfg.offset_y = 0;        // Panel offset in Y direction
#ifdef PORTRAIT
      cfg.offset_rotation = 0;
#else
      cfg.offset_rotation = 6;
#endif
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

      cfg.pin_bl = 15;     // pin number to which the backlight is connected
      cfg.invert = true;  // true to invert backlight brightness
      cfg.freq = 44100;    // backlight PWM frequency
      cfg.pwm_channel = 1; // PWM channel number to use

      _light_instance.config(cfg);
      // Sets the backlight to the panel.
      _panel_instance.setLight(&_light_instance);
    }

    setPanel(&_panel_instance); // Sets the panel to use.
  }
};

#endif