#include "ssd1305_int.h"

#pragma extrn Cspi_control
#pragma extrn Cspi_write
#pragma extrn Cspi_write_dma
#pragma extrn C_ssd1305_window

/*
 * Show an image of SSD1305_FRAME_SIZE bytes.  The spi functions are
 * called here, and not by way of _ssd1305_cmd, to keep the time for
 * an image short.
 */
void ssd1305_display(const byte *frame) {
  spi_control(OLED_COMMAND);
  spi_write(_ssd1305_window, WINDOW_SIZE);
  spi_control(OLED_DATA);
  spi_write_dma(frame, SSD1305_FRAME_SIZE);
  spi_control(OLED_IDLE);
}
