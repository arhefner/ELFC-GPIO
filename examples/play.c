/*
 * play.c - plays a movie on an SSD1305 OLED display
 *
 * Shows the images of a file one after the other, 32 of them in a
 * second, and starts over at the end of the file, until the input
 * button is pressed.  The file is images of 1024 bytes, see
 * ssd1305/ssd1305.h for what they are.
 *
 *   play sample1.mov
 *
 * The time for an image is two ticks of the real time clock, which
 * the tick functions of the gpio library count.  This needs the real
 * time clock of the 1802/Mini or the 1802/MAX.
 *
 * Build for Elf/OS with:   make
 * Build for ELF-DOS with:  make TARGET_OS=elfdos
 *
 * The ports of the SPI adapter and the real time clock are those of
 * sysconfig.h.  The input button is on EF4 unless another line is
 * given:
 *
 *   make BUTTON=ef1_asserted
 */
#include <stdio.h>
#include <stdlib.h>
#include "../include/gpio.h"
#include "../include/sysconfig.h"
#include "ssd1305/ssd1305.h"

#define SPI_CTRL_PORT  PORT(SPI_GROUP, SPI_CTL)
#define SPI_DATA_PORT  PORT(SPI_GROUP, SPI_DATA)
#define RTC            PORT(RTC_GROUP, RTC_PORT)

#ifndef BUTTON_PRESSED
#define BUTTON_PRESSED  ef4_asserted
#endif

/* 32 images in a second */
#define TICKS_PER_FRAME  (TICKS_PER_SECOND / 32)

static byte frame[SSD1305_FRAME_SIZE];

int main(int argc, char *argv[]) {
  int fd;
  int frames;

  if (argc != 2) {
    printf("Usage: play filename\n");
    return 1;
  }

  fd = open(argv[1], O_RDONLY);
  if (fd < 0) {
    printf("File not found.\n");
    return 1;
  }

  spi_init(SPI_CTRL_PORT, SPI_DATA_PORT);
  ssd1305_init();
  ssd1305_clear();

  tick_start(RTC);

  /*
   * An image is read first and shown when it is time for it.  If
   * reading it took longer than that, it is shown at once, and the
   * movie plays as fast as the file can be read.
   */
  frames = 0;
  while (!BUTTON_PRESSED()) {
    if (read(fd, frame, SSD1305_FRAME_SIZE) != SSD1305_FRAME_SIZE) {
      if (frames == 0) {
        tick_stop();
        printf("No image in the file.\n");
        close(fd);
        return 1;
      }
      lseek(fd, 0, SEEK_SET);
      frames = 0;
      continue;
    }
    frames++;

    tick_wait(TICKS_PER_FRAME);
    ssd1305_display(frame);
  }

  tick_stop();
  close(fd);

  return 0;
}
