#ifndef _LED7_INT_
#define _LED7_INT_

/*
 * Definitions shared by the functions of the led7 library.
 *
 * A function of an ElfC library has to name the functions and the
 * variables of other procedures that it uses, as _ELFCLIB_ keeps the
 * header files from doing so.  Each source file has a #pragma extrn
 * for the ones it uses.
 *
 * The library is the same for every i2c driver, the program it is
 * linked with names the driver.  I2C_OTHER_DRIVER gets the i2c
 * functions declared here without naming one.
 */
#define _ELFCLIB_
#define I2C_OTHER_DRIVER
#include "../../include/gpio.h"
#include "led7.h"

/* commands of the HT16K33 */
#define LED7_CMD_OSC_OFF  0x20
#define LED7_CMD_OSC_ON   0x21
#define LED7_CMD_DISP_OFF 0x80
#define LED7_CMD_DISP_ON  0x81
#define LED7_CMD_BRIGHT   0xe0

/*
 * The buffer is the message that sets the display memory: the address
 * in the memory, which is 0, and then the bytes of the memory.  The
 * digits are the bytes 0, 2, 6 and 8 of the memory and the colon is
 * byte 4, so with the address before them they are at these places
 * in the buffer.
 */
#define LED7_BUF_SIZE  11
#define LED7_COLON     5
#define LED7_DIGIT(pos)  ((pos) < 2 ? (pos) * 2 + 1 : (pos) * 2 + 3)

extern byte _led7_addr;
extern byte _led7_buf[LED7_BUF_SIZE];

/* send a one byte command to the display */
bool _led7_cmd(byte cmd);

#endif
