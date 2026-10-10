#include "ssd1305_int.h"

#pragma extrn C_ssd1305_cmd

/* Turn the display on again, with the image it had. */
void ssd1305_wake(void) {
  byte cmd;

  cmd = SET_DISP_ON;
  _ssd1305_cmd(&cmd, 1);
}
