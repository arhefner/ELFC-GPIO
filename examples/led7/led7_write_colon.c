#include "led7_int.h"

#pragma extrn C_led7_buf

/* Turn the colon on or off in the buffer. */
void led7_write_colon(bool on) {
  _led7_buf[LED7_COLON] = on ? 0x02 : 0;
}
