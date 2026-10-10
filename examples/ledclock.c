/*
 * ledclock.c - shows the time on a seven segment LED display
 *
 * Shows the time of day on an HT16K33 display on the i2c bus for a
 * number of seconds, then turns the display off.  The number of
 * seconds is the argument, 10 if there is none:
 *
 *   ledclock 30
 *
 * The time is shown as hours and minutes on a twelve hour clock,
 * with the colon on in the even seconds.
 *
 * Build for Elf/OS with:   make
 * Build for ELF-DOS with:  make TARGET_OS=elfdos
 *
 * The i2c driver is named when the program is built, the makefile
 * does that:
 *
 *   make I2C=BB
 *   elfc -D I2C_BB -D SYS_1802MAX ledclock.c
 *
 * The port of the i2c bus is I2C_GROUP and I2C_PORT of sysconfig.h.
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../include/gpio.h"
#include "../include/sysconfig.h"
#include "led7/led7.h"

#define I2C_BUS  PORT(I2C_GROUP, I2C_PORT)

#define DEFAULT_SECONDS  10

/*
 * The clock is read every POLL_MS, and the display is changed when
 * the second has changed.  The seconds are counted by the clock, not
 * by the delays, which are longer by the time it takes to read the
 * clock.  If the second has not changed after STALLED_POLLS, which
 * is more than a second, the clock is not running, and a second is
 * counted anyway so that the program ends.
 */
#define POLL_MS        100
#define STALLED_POLLS  12

static struct tm now;

/* show the time in now, false if the display did not answer */
static bool show_time(void) {
  int hour;

  hour = now.tm_hour % 12;
  if (hour == 0)
    hour = 12;

  if (hour < 10)
    led7_write_blank(0);
  else
    led7_write_digit(0, hour / 10);
  led7_write_digit(1, hour % 10);
  led7_write_digit(2, now.tm_min / 10);
  led7_write_digit(3, now.tm_min % 10);
  led7_write_colon((now.tm_sec & 1) == 0);

  return led7_write_disp();
}

int main(int argc, char *argv[]) {
  int seconds;
  int shown;
  int polls;

  seconds = DEFAULT_SECONDS;
  if (argc > 1)
    seconds = atoi(argv[1]);
  if (seconds < 1) {
    printf("Usage: ledclock [seconds]\n");
    return 1;
  }

  if (!i2c_init(I2C_BUS)) {
    printf("The i2c bus is not free\n");
    return 1;
  }
  if (!led7_init(LED7_ADDR)) {
    printf("No display at i2c address %x\n", LED7_ADDR);
    return 1;
  }

  shown = -1;
  polls = 0;
  while (seconds > 0) {
    systime(&now);
    if (now.tm_sec != shown) {
      if (shown >= 0)
        seconds--;
      shown = now.tm_sec;
      polls = 0;
      if (!show_time()) {
        printf("The display stopped answering\n");
        return 1;
      }
    } else if (++polls == STALLED_POLLS) {
      seconds--;
      polls = 0;
    }
    delay_ms(POLL_MS);
  }

  led7_off();

  return 0;
}
