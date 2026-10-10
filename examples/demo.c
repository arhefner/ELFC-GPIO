/*
 * demo.c - demonstration of the gpio library
 *
 * Counts on the LEDs of port 4 until the input button is pressed,
 * then shows the EF lines and the switches of port 4.
 *
 * Build for Elf/OS with:   make
 * Build for ELF-DOS with:  make TARGET_OS=elfdos
 *
 * The input button is on EF4 unless another line is given:
 *
 *   make BUTTON=ef1_asserted
 *   elfc -D BUTTON_PRESSED=ef1_asserted demo.c
 */
#include <stdio.h>
#include "../include/gpio.h"
#include "../include/sysconfig.h"

/* the hex display and the switches, in the default port group */
#define LEDS      PORT(PIO1_GROUP, PIO1_OUTB)
#define SWITCHES  PORT(PIO1_GROUP, 4)

/*
 * The function that tests the line the input button is on.  ElfC has
 * no #if to choose it by a number, so the symbol is the name of the
 * function itself, and the call is the same as calling it directly.
 */
#ifndef BUTTON_PRESSED
#define BUTTON_PRESSED  ef4_asserted
#endif

int main(void) {
  byte count;

  printf("Counting, press the input button to stop\n");

  count = 0;
  while (!BUTTON_PRESSED()) {
    out_port(LEDS, ~(count++));
    delay_ms(250);
  }

  printf("EF1 %d  EF2 %d  EF3 %d  EF4 %d\n",
         ef1_asserted(), ef2_asserted(), ef3_asserted(), ef4_asserted());
  printf("Switches %x\n", inp_port(SWITCHES));

  printf("Waiting two seconds\n");
  delay_s(2);
  out_port(LEDS, ~0);

  return 0;
}
