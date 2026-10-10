#include "ssd1305_int.h"

#pragma extrn Cspi_control
#pragma extrn Cspi_write

/* Send commands to the display. */
void _ssd1305_cmd(const byte *cmds, unsigned int len) {
  spi_control(OLED_COMMAND);
  spi_write(cmds, len);
  spi_control(OLED_IDLE);
}
