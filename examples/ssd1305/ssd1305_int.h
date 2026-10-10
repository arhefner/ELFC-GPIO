#ifndef _SSD1305_INT_
#define _SSD1305_INT_

/*
 * Definitions shared by the functions of the ssd1305 library.
 *
 * A function of an ElfC library has to name the functions and the
 * variables of other procedures that it uses, as _ELFCLIB_ keeps the
 * header files from doing so.  Each source file has a #pragma extrn
 * for the ones it uses.
 */
#define _ELFCLIB_
#include "../../include/gpio.h"
#include "ssd1305.h"

/*
 * The lines of the SPI adapter.  The reset line of the display is
 * low to reset it, so it is high in all but OLED_RESET.
 */
#define OLED_IDLE     SPI_OUT1
#define OLED_RESET    SPI_OUT0
#define OLED_COMMAND  (SPI_CS0 | SPI_OUT1)
#define OLED_DATA     (SPI_CS0 | SPI_OUT1 | SPI_OUT0)

/* commands of the SSD1305 */
#define SET_MEM_ADDR_MODE    0x20
#define SET_COL_ADDR         0x21
#define SET_PAGE_ADDR        0x22
#define SET_DISP_START_LINE  0x40
#define SET_CONTRAST         0x81
#define SET_SEG_REMAP        0xa0
#define SET_ENTIRE_ON        0xa4
#define SET_NORM_INV         0xa6
#define SET_MUX_RATIO        0xa8
#define SET_DISP_OFF         0xae
#define SET_DISP_ON          0xaf
#define SET_COM_OUT_DIR      0xc0
#define SET_DISP_OFFSET      0xd3
#define SET_DISP_CLK_DIV     0xd5
#define SET_PRECHARGE        0xd9
#define SET_COM_PIN_CFG      0xda
#define SET_VCOM_DESEL       0xdb

#define HORIZONTAL_MODE      0x00

/*
 * The commands that set where the bytes of an image go, which is
 * all pages and all columns.
 */
#define WINDOW_SIZE  6

extern byte _ssd1305_window[WINDOW_SIZE];

/* send commands to the display */
void _ssd1305_cmd(const byte *cmds, unsigned int len);

#endif
