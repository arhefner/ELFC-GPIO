#include "led7_int.h"

#pragma extrn C_led7_buf

/* Turn the segments of a digit off in the buffer, pos is 0 to 3. */
void led7_write_blank(byte pos) {
  pos &= 3;
  _led7_buf[LED7_DIGIT(pos)] = 0;
}
