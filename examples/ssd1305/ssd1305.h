#ifndef _SSD1305_
#define _SSD1305_

/*
 * ssd1305.h - library for a 128 by 64 OLED display with an SSD1305
 * controller on the SPI adapter of the 1802/Mini and the 1802/MAX
 *
 * The display is on chip select 0 of the adapter.  General output 0
 * is its data/command line and general output 1 is its reset line.
 *
 * The library uses the spi functions of the gpio library.  Include
 * gpio.h before this file, and call spi_init before any of these
 * functions.
 *
 * An image for the display is SSD1305_FRAME_SIZE bytes.  It is the
 * eight rows of eight pixels from the top down, and each row is a
 * byte for each column from left to right, with the top pixel of
 * the eight in bit 0.
 *
 * Including this file in a program makes the linker search
 * ssd1305.lib, which has to be in the lib folder of ElfC or in the
 * current folder.
 */
#ifndef _ELFCLIB_
#pragma .link .library ssd1305.lib
#pragma .link .requires Cssd1305_init
#endif

#define SSD1305_WIDTH       128
#define SSD1305_HEIGHT      64
#define SSD1305_FRAME_SIZE  1024

/* reset the display and turn it on, the image is not cleared */
void ssd1305_init(void);

/* clear the image */
void ssd1305_clear(void);

/* show an image of SSD1305_FRAME_SIZE bytes */
void ssd1305_display(const byte *frame);

/* set the contrast, 0 to 255 */
void ssd1305_set_contrast(byte level);

/* turn the display off, it keeps its image, and on again */
void ssd1305_sleep(void);
void ssd1305_wake(void);

/*
 * make an image for the display from a Pixie image, the 64 by 32
 * image of the CDP1861 video chip, with every pixel two wide and
 * two high
 */
#define SSD1305_PIXIE_SIZE  256

void ssd1305_pixie(const byte *pixie, byte *frame);

#endif
