#include "led7_int.h"

#pragma extrn C_led7_cmd

/* Set the brightness, 0 to 15. */
bool led7_set_brightness(byte level) {
  return _led7_cmd(LED7_CMD_BRIGHT | (level & 0x0f));
}
