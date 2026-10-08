#ifndef _GPIO_
#define _GPIO_

/*
 * gpio.h - hardware access library for ElfC
 *
 * The functions in this library test the EF lines, read and write the
 * I/O ports, and wait for a time.  They make no BIOS or operating
 * system calls, so a program that uses them runs under both Elf/OS
 * and ELF-DOS.
 *
 * Including this file in a program makes the linker search gpio.lib,
 * which has to be in the lib folder of ElfC or in the current folder.
 */
#ifndef _ELFCLIB_
#pragma .link .library gpio.lib
#endif

#include <stdbool.h>

typedef unsigned char byte;

/*
 * A port is its port group and its port index, 1 to 7, in one value.
 * ElfC passes every argument as two bytes, so this takes one argument
 * where the group and the index on their own would take two, and PORT
 * with constants is a constant, so nothing is calculated at run time:
 *
 *   #define LEDS  PORT(0, 4)
 *
 *   out_port(LEDS, 0x55);
 *
 * Group 0 is the default group.  Use it on a system without a port
 * group expander.
 */
typedef unsigned int port_t;

#define PORT(group, index)  (((group) << 8) | (index))

/* the external flag lines, true if the line is asserted (pin is low) */
bool ef1_asserted(void);
bool ef2_asserted(void);
bool ef3_asserted(void);
bool ef4_asserted(void);

/* the ports */
void out_port(port_t port, byte value);
byte inp_port(port_t port);

/*
 * set the port that selects the port group, for a system where it is
 * not the one the library was built for, which is port 5 by default
 */
void set_exp_port(byte port);

/* wait for a number of milliseconds or seconds, 0 to 65535 */
void delay_ms(unsigned int ms);
void delay_s(unsigned int s);

#endif
