#include "led7_int.h"

#pragma extrn Ci2c_write
#pragma extrn C_led7_addr
#pragma extrn C_led7_buf

/* Send the buffer to the display. */
bool led7_write_disp(void) {
  return i2c_write(_led7_addr, _led7_buf, LED7_BUF_SIZE);
}
