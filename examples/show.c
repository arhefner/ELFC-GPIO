/*
 * show.c - shows an image on an SSD1305 OLED display
 *
 * Shows the image of a file.  The file is either an image for the
 * display, which is 1024 bytes, or a Pixie image, which is 256 bytes
 * and is made twice as wide and as high to fill the display.  See
 * ssd1305/ssd1305.h for what they are.
 *
 *   show hires.img
 *
 * Build for Elf/OS with:   make
 * Build for ELF-DOS with:  make TARGET_OS=elfdos
 *
 * The ports of the SPI adapter are those of sysconfig.h.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/gpio.h"
#include "../include/sysconfig.h"
#include "ssd1305/ssd1305.h"

#define SPI_CTRL_PORT  PORT(SPI_GROUP, SPI_CTL)
#define SPI_DATA_PORT  PORT(SPI_GROUP, SPI_DATA)

static byte frame[SSD1305_FRAME_SIZE];
static byte pixie[SSD1305_PIXIE_SIZE];

int main(int argc, char *argv[]) {
  int fd;
  int size;
  byte more;

  if (argc != 2) {
    printf("Usage: show filename\n");
    return 1;
  }

  fd = open(argv[1], O_RDONLY);
  if (fd < 0) {
    printf("File not found.\n");
    return 1;
  }

  /* the size of the file tells what it is, so look for a byte more */
  size = read(fd, frame, SSD1305_FRAME_SIZE);
  if (size == SSD1305_FRAME_SIZE && read(fd, &more, 1) == 1)
    size++;
  close(fd);

  if (size < 0) {
    printf("File read error.\n");
    return 1;
  }

  if (size == SSD1305_PIXIE_SIZE) {
    memcpy(pixie, frame, SSD1305_PIXIE_SIZE);
    ssd1305_pixie(pixie, frame);
  } else if (size != SSD1305_FRAME_SIZE) {
    printf("Invalid bitmap.\n");
    return 1;
  }

  spi_init(SPI_CTRL_PORT, SPI_DATA_PORT);
  ssd1305_init();
  ssd1305_display(frame);

  return 0;
}
