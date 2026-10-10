#include "ssd1305_int.h"

#pragma extrn C_ssd1305_cmd

/* Turn the display off.  It keeps its image. */
void ssd1305_sleep(void) {
  byte cmd;

  cmd = SET_DISP_OFF;
  _ssd1305_cmd(&cmd, 1);
}
