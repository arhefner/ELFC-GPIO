#include "led7_int.h"

#pragma extrn C_led7_cmd

/*
 * Stop the display.  The display is turned off and then its
 * oscillator is stopped, which puts the HT16K33 in standby.
 */
bool led7_off(void) {
  return _led7_cmd(LED7_CMD_DISP_OFF) && _led7_cmd(LED7_CMD_OSC_OFF);
}
