# RomerinBody ESP32-S3-Touch-LCD-1.28
This project uses the following libraries:
1. [LVGL - Light and Versatile Graphics Library v9.2.3](https://github.com/lvgl/lvgl)
2. [TFT_eSPI v2.5.43](https://github.com/Bodmer/TFT_eSPI)
3. [CST816S v1.1.1](https://github.com/fbiego/CST816S)

## `platformio.ini` file specification
Important considerations regarding `platformio.ini`, you'll see that has the following `build_flags`:
  ```bash
  -D USER_SETUP_LOADED=1                        ; Set this settings as valid
  -include $PROJECT_LIBDEPS_DIR/$PIOENV/TFT_eSPI/User_Setups/Setup302_Waveshare_ESP32S3_GC9A01.h
  -D USE_HSPI_PORT=1                            ; Fix for when screen doesn't boot up
  -D LV_CONF_PATH="${PROJECT_DIR}/src/config/lv_conf.h"
  ```

1. These are to configure the SPI Pins to communicate with the LCD 1.28 Touch Screen and `USE_HSPI_PORT` to fix the issue for the screen not booting up.
2. [LVGL - Light and Versatile Graphics Library](https://github.com/lvgl/lvgl) - `lv_conf.h` is to configure the library compilation with some custom configuration.


https://www.waveshare.com/wiki/ESP32-S3-Touch-LCD-1.28

based on: https://github.com/y3rbiadit0/esp32-S3-Touch-LCD-1.28-sample-pio-project

