#include "led7_int.h"

#pragma extrn C_led7_cmd

/*
 * Set the blink rate, one of the LED7_BLINK rates.  The rate is in
 * the command that turns the display on.
 */
bool led7_set_blink_rate(byte rate) {
  return _led7_cmd(LED7_CMD_DISP_ON | ((rate & 3) << 1));
}
