#include "ssd1305_int.h"

#pragma extrn Cspi_control
#pragma extrn Cspi_write_dma
#pragma extrn C_ssd1305_cmd
#pragma extrn C_ssd1305_window

/* a row of the image with all pixels off */
static byte blank[SSD1305_WIDTH];

/* Clear the image. */
void ssd1305_clear(void) {
  int row;

  _ssd1305_cmd(_ssd1305_window, WINDOW_SIZE);

  spi_control(OLED_DATA);
  for (row = 0; row < SSD1305_HEIGHT / 8; row++)
    spi_write_dma(blank, SSD1305_WIDTH);
  spi_control(OLED_IDLE);
}
