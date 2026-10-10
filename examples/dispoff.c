/*
 * dispoff.c - clears and turns off an SSD1305 OLED display
 *
 * Clears the memory of an SSD1305 OLED display and puts
 * it to sleep.
 *
 * Build for Elf/OS with:   make
 * Build for ELF-DOS with:  make TARGET_OS=elfdos
 *
 * The ports of the SPI adapter are those of sysconfig.h.
 */
#include <stdio.h>
#include <stdlib.h>
#include "../include/gpio.h"
#include "../include/sysconfig.h"
#include "ssd1305/ssd1305.h"

#define SPI_CTRL_PORT  PORT(SPI_GROUP, SPI_CTL)
#define SPI_DATA_PORT  PORT(SPI_GROUP, SPI_DATA)

int main(int argc, char *argv[]) {

  spi_init(SPI_CTRL_PORT, SPI_DATA_PORT);

  ssd1305_init();
  ssd1305_clear();
  ssd1305_sleep();

  return 0;
}
