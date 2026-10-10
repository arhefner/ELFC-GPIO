#include "led7_int.h"

#pragma extrn C_led7_cmd
#pragma extrn Cled7_clear
#pragma extrn Cled7_write_disp

/* the i2c address of the display and the buffer, see led7_int.h */
byte _led7_addr;
byte _led7_buf[LED7_BUF_SIZE];

/*
 * Start the display at an i2c address, with all segments off, full
 * brightness and no blinking.
 */
bool led7_init(byte addr) {
  _led7_addr = addr;

  if (!_led7_cmd(LED7_CMD_OSC_ON))
    return false;

  led7_clear();

  return led7_write_disp()
      && _led7_cmd(LED7_CMD_BRIGHT | 15)
      && _led7_cmd(LED7_CMD_DISP_ON);
}
