/*
 * demo.c - demonstration of the gpio library
 *
 * Counts on the LEDs of port 4 until the input button on EF4 is
 * pressed, then shows the EF lines and the switches of port 4.
 *
 * Build for Elf/OS with:   make
 * Build for ELF-DOS with:  make TARGET_OS=elfdos
 */
#include <stdio.h>
#include "../include/gpio.h"

/* the hex display and the switches, in the default port group */
#define LEDS      PORT(0, 4)
#define SWITCHES  PORT(0, 4)

int main(void) {
  byte count;

  printf("Counting, press the input button to stop\n");

  count = 0;
  while (!ef4_asserted()) {
    out_port(LEDS, count++);
    delay_ms(250);
  }

  printf("EF1 %d  EF2 %d  EF3 %d  EF4 %d\n",
         ef1_asserted(), ef2_asserted(), ef3_asserted(), ef4_asserted());
  printf("Switches %x\n", inp_port(SWITCHES));

  printf("Waiting two seconds\n");
  delay_s(2);
  out_port(LEDS, 0);

  return 0;
}
