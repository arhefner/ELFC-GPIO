#include "led7_int.h"

#pragma extrn C_led7_buf

/* the segments of the digits 0 to F, segment a is bit 0 */
static byte segments[16] = {
  0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07,
  0x7f, 0x6f, 0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71
};

/* Put a digit in the buffer, pos is 0 to 3 and value is 0 to 15. */
void led7_write_digit(byte pos, byte value) {
  pos &= 3;
  _led7_buf[LED7_DIGIT(pos)] = segments[value & 0x0f];
}
