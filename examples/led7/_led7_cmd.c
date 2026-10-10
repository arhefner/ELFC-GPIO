#include "led7_int.h"

#pragma extrn Ci2c_write
#pragma extrn C_led7_addr

/* Send a one byte command to the display. */
bool _led7_cmd(byte cmd) {
  return i2c_write(_led7_addr, &cmd, 1);
}
