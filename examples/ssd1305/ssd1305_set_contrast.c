#include "ssd1305_int.h"

#pragma extrn C_ssd1305_cmd

/* Set the contrast, 0 to 255. */
void ssd1305_set_contrast(byte level) {
  byte cmds[2];

  cmds[0] = SET_CONTRAST;
  cmds[1] = level;
  _ssd1305_cmd(cmds, 2);
}
