#ifndef _GPIO_
#define _GPIO_

/*
 * gpio.h - hardware access library for ElfC
 *
 * The functions in this library test the EF lines, read and write the
 * I/O ports, wait for a time, count the ticks of the real time clock,
 * and talk to the devices on an i2c bus and on an SPI adapter.
 * They make no BIOS or operating system calls, so a program that uses
 * them runs under both Elf/OS and ELF-DOS.
 *
 * Including this file in a program makes the linker search gpio.lib,
 * which has to be in the lib folder of ElfC or in the current folder.
 */
#ifndef _ELFCLIB_
#pragma .link .library gpio.lib
#endif

/*
 * The i2c functions are only declared if the program names the driver
 * for its i2c hardware, which is a library of its own.  It does so by
 * defining one of these symbols before it includes this file, or with
 * the -D option of ElfC:
 *
 *   I2C_BB             i2c_bb.lib, the bit-banged driver
 *   I2C_OTHER_DRIVER   a driver that is not known here, the program
 *                      names its library itself
 *
 *   elfc -D I2C_BB prog.c
 *
 *   #define I2C_OTHER_DRIVER
 *   #pragma .link .library i2c_mine.lib
 *   #include <gpio.h>
 *
 * A program that defines none of them needs no driver library.
 */
#ifdef I2C_BB
#ifndef _ELFCLIB_
#pragma .link .library i2c_bb.lib
#endif
#ifndef _GPIO_I2C_
#define _GPIO_I2C_
#endif
#endif

#ifdef I2C_OTHER_DRIVER
#ifndef _GPIO_I2C_
#define _GPIO_I2C_
#endif
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

/*
 * The spi functions are for the SPI adapter of the 1802/Mini and the
 * 1802/MAX.  spi_init sets its control port and its data port, which
 * have to be in the same group, and has to be called before the
 * others.  If they are not in group 0, call set_exp_port first on a
 * system where that is needed.
 *
 *   spi_init(PORT(0, 2), PORT(0, 3));
 *
 * spi_control sets the lines of the adapter, the two chip selects
 * and the two general outputs.  The lines that are named are high,
 * and a chip is selected when its line is named:
 *
 *   spi_control(SPI_CS0 | SPI_OUT1);
 *
 * Do not leave a chip selected while calling the operating system,
 * which uses the adapter for the micro SD card on some systems.
 */
#define SPI_OUT0  0x01
#define SPI_OUT1  0x02
#define SPI_CS0   0x04
#define SPI_CS1   0x08

void spi_init(port_t ctrl, port_t data);
void spi_control(byte lines);

/*
 * spi_write sends len bytes.
 *
 * spi_read receives len bytes, sending 0xff for each of them.
 *
 * spi_transfer sends len bytes and stores the byte that came in with
 * each of them.  The two pointers may be the same.
 */
void spi_write(const byte *data, unsigned int len);
void spi_read(byte *data, unsigned int len);
void spi_transfer(const byte *wdata, byte *rdata, unsigned int len);

/*
 * These do the same as spi_write and spi_read with DMA, which takes
 * one machine cycle for a byte where the others take ten or more.
 * They are for a lot of bytes, such as the image of a display.  For
 * a few bytes they are no faster, as they take longer to start.
 *
 * They use R0, the pointer for DMA.
 *
 * spi_read_dma clocks one more byte out of the device than it
 * stores, which is how the adapter receives with DMA.  Use spi_read
 * for a device that this is a problem for.
 */
void spi_write_dma(const byte *data, unsigned int len);
void spi_read_dma(byte *data, unsigned int len);

/*
 * The tick functions count the interrupts of the real time clock of
 * the 1802/Mini and the 1802/MAX, which they set to interrupt 64
 * times in a second.
 *
 * tick_start starts the ticks, with the port of the clock, and
 * tick_stop stops them and leaves the clock as the bios has it.  If
 * the port is not in group 0, call set_exp_port first on a system
 * where that is needed.
 *
 *   tick_start(PORT(0, 1));
 *   while (!ef4_asserted()) {
 *     tick_wait(TICKS_PER_SECOND / 2);
 *     ...                               twice in a second
 *   }
 *   tick_stop();
 *
 * tick_wait waits until n ticks after the time the wait before it
 * ended, so the time between two waits does not depend on what is
 * done between them, as long as that takes less than n ticks.
 *
 * ticks is the number of ticks so far, which starts over after 255.
 *
 * The ticks use the interrupt and R1, so nothing else can.  Do not
 * read or set the time of the clock while they are running, as the
 * interrupt routine writes to the clock as well.  tick_stop leaves
 * interrupts disabled.
 */
#define TICKS_PER_SECOND  64

void tick_start(port_t rtc);
void tick_stop(void);
byte ticks(void);
void tick_wait(byte n);

#ifdef _GPIO_I2C_

/*
 * i2c_init sets the port of the i2c hardware, and has to be called
 * before the others.  It frees the bus as i2c_clear does.  If the
 * port is not in group 0, call set_exp_port first on a system where
 * that is needed.
 *
 * With the bit-banged driver, the bus is two bits of an output port,
 * bit 7 for the data line and bit 6 for the clock line, with the data
 * line read back on EF3 and the clock line on EF4.  The other bits of
 * the port are written as 0.
 *
 *   i2c_init(PORT(1, 2));
 *
 * i2c_clear frees the bus from a device that is holding the data line
 * low because it was left in the middle of a byte.  It is true if the
 * data line is high afterwards.
 */
bool i2c_init(port_t port);
bool i2c_clear(void);

/*
 * These send a message to the device with the 7 bit address addr, or
 * receive one from it.  They are true if the device acknowledged its
 * address and all the bytes that were sent to it.
 *
 * i2c_transfer sends wlen bytes and then, after a repeated start,
 * receives rlen bytes.  This is how a register of a device is read:
 *
 *   byte reg = 0x05;
 *   byte value[2];
 *
 *   if (!i2c_transfer(0x48, &reg, 1, value, 2)) ...
 *
 * With a length of 0 only the address is sent, which shows if there
 * is a device with that address.
 */
bool i2c_write(byte addr, const byte *data, unsigned int len);
bool i2c_read(byte addr, byte *data, unsigned int len);
bool i2c_transfer(byte addr, const byte *wdata, unsigned int wlen,
                  byte *rdata, unsigned int rlen);

#endif

#endif
