#include "ssd1305_int.h"

#pragma extrn Cspi_control
#pragma extrn Cdelay_ms
#pragma extrn C_ssd1305_cmd

/* see ssd1305_int.h */
byte _ssd1305_window[WINDOW_SIZE] = {
  SET_PAGE_ADDR, 0, SSD1305_HEIGHT / 8 - 1,
  SET_COL_ADDR, 0, SSD1305_WIDTH - 1
};

/* the commands that set the display up, the last one turns it on */
#define SETUP_SIZE  23

static byte setup[SETUP_SIZE] = {
  SET_DISP_OFF,
  SET_MEM_ADDR_MODE, HORIZONTAL_MODE,
  SET_DISP_START_LINE | 0,
  SET_SEG_REMAP | 1,              /* column 127 is segment 0 */
  SET_MUX_RATIO, SSD1305_HEIGHT - 1,
  SET_COM_OUT_DIR | 8,            /* scan from the last row to row 0 */
  SET_DISP_OFFSET, 0,
  SET_COM_PIN_CFG, 0x12,
  SET_DISP_CLK_DIV, 0x80,
  SET_PRECHARGE, 0xf1,
  SET_VCOM_DESEL, 0x3c,
  SET_CONTRAST, 0x80,
  SET_ENTIRE_ON,
  SET_NORM_INV,
  SET_DISP_ON
};

/* Reset the display and turn it on.  The image is not cleared. */
void ssd1305_init(void) {
  spi_control(OLED_IDLE);
  delay_ms(1);
  spi_control(OLED_RESET);
  delay_ms(10);
  spi_control(OLED_IDLE);
  delay_ms(10);

  _ssd1305_cmd(setup, SETUP_SIZE);
}
