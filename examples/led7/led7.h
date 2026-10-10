#ifndef _LED7_
#define _LED7_

/*
 * led7.h - library for a four digit seven segment LED display with a
 * colon, driven by an HT16K33 on the i2c bus, such as the Adafruit
 * 0.56" seven segment backpack
 *
 * The library uses the i2c functions of the gpio library.  Include
 * gpio.h before this file, with an i2c driver named, and call
 * i2c_init before any of these functions.
 *
 * The digits and the colon are kept in a buffer.  The functions that
 * change them change the buffer only, and led7_write_disp sends the
 * buffer to the display, so the display changes all at once.
 *
 * The functions that send to the display are true if the display
 * acknowledged the message.
 *
 * Including this file in a program makes the linker search led7.lib,
 * which has to be in the lib folder of ElfC or in the current folder.
 */
#ifndef _ELFCLIB_
#pragma .link .library led7.lib
#pragma .link .requires Cled7_init
#endif

/* the address of the display when no address jumper is closed */
#define LED7_ADDR  0x70

/* blink rates for led7_set_blink_rate */
#define LED7_BLINK_OFF     0
#define LED7_BLINK_2HZ     1
#define LED7_BLINK_1HZ     2
#define LED7_BLINK_HALFHZ  3

/*
 * start the display at an i2c address, with all segments off, full
 * brightness and no blinking
 */
bool led7_init(byte addr);

/* stop the display, which turns it off until led7_init is called */
bool led7_off(void);

/* set the brightness, 0 to 15 */
bool led7_set_brightness(byte level);

/* set the blink rate, one of the rates above */
bool led7_set_blink_rate(byte rate);

/*
 * Change the buffer.  The digits are numbered 0 to 3 from the left,
 * and the value of a digit is 0 to 15, shown in hexadecimal.
 */
void led7_clear(void);
void led7_write_digit(byte pos, byte value);
void led7_write_blank(byte pos);
void led7_write_colon(bool on);

/* send the buffer to the display */
bool led7_write_disp(void);

#endif
