#include "led7_int.h"

#pragma extrn C_led7_buf

/* Turn all segments off in the buffer. */
void led7_clear(void) {
  int i;

  for (i = 0; i < LED7_BUF_SIZE; i++)
    _led7_buf[i] = 0;
}
