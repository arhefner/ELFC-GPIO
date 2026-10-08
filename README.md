# ELFC-GPIO

A hardware access library for the [ElfC](https://github.com/arhefner/ELFC)
C compiler for the CDP1802. It tests the EF lines, reads and writes the I/O
ports, and waits for a time.

The library makes no BIOS or operating system calls, and calls nothing in the
ElfC runtime, so the same `gpio.lib` works in programs for both Elf/OS and
ELF-DOS.

The functions are written in assembly, not in C with inline assembly. They
take their arguments straight from the expression stack and have no stack
frame, which keeps them small and fast.

## Building

ElfC has to be on the path.

```
make                 build lib/gpio.lib
make examples        build the example program for Elf/OS
make install         copy gpio.lib and gpio.h into ElfC (default /opt/elfc)
make clean           remove the generated files
```

Use `make install ELFC_DIR=folder` if ElfC is installed somewhere else, and
`make -C examples TARGET_OS=elfdos` to build the example for ELF-DOS.

Two settings are built into the library. Run `make clean` before changing
them:

| Setting | Default | Description |
|---------|---------|-------------|
| `EXP_PORT` | 5 | The port that selects the port group. It is 5 on the 1802/Mini, 1802/MAX and Super Elf, and 1 on most other systems. A program can change it with `set_exp_port`. |
| `FREQ_KHZ` | 4000 | The clock frequency the delay functions assume if the BIOS has not stored the real one. |

```
make EXP_PORT=1 FREQ_KHZ=1790
```

## Using the library

Include `gpio.h` in the program. The header file tells the linker to search
`gpio.lib`, so no linker option is needed:

```c
#include <gpio.h>

#define LEDS  PORT(0, 4)

int main(void) {
  byte count = 0;

  while (!ef4_asserted()) {
    out_port(LEDS, count++);
    delay_ms(250);
  }
  return 0;
}
```

```
elfc blink.c         for Elf/OS
elfc -E blink.c      for ELF-DOS
```

The linker looks for `gpio.lib` in the `lib` folder of ElfC and in the
current folder. Without `make install`, copy `lib/gpio.lib` next to the
program and include the header file by its path.

## Functions

| Function | Description |
|----------|-------------|
| `bool ef1_asserted(void)` | True if the EF1 line is asserted. Also `ef2_asserted`, `ef3_asserted` and `ef4_asserted`. |
| `void out_port(port_t port, byte value)` | Write a byte to a port. |
| `byte inp_port(port_t port)` | Read a byte from a port. |
| `void set_exp_port(byte port)` | Set the port that selects the port group, 1 to 7. |
| `void delay_ms(unsigned int ms)` | Wait for 0 to 65535 milliseconds. |
| `void delay_s(unsigned int s)` | Wait for 0 to 65535 seconds. |

`byte` is `unsigned char`, and `bool` comes from `stdbool.h`.

## Ports

A port is a `port_t`, an `unsigned int` with the port group in the high byte
and the port index, 1 to 7, in the low byte. The `PORT(group, index)` macro
makes one:

```c
#define UART_DATA  PORT(0, 6)
#define RTC        PORT(1, 3)
```

The group and the index are in one value because of how ElfC calls a
function. Every argument is pushed as two bytes, whatever its type, so three
`byte` arguments cost three pushes. A structure costs more still, as it is
copied onto the stack by a runtime routine. With a constant group and index,
`PORT` is a constant that is pushed once. These are the sizes of a call with
a constant port and the value in a global variable:

| `out_port` takes | Bytes for the call | Pushes |
|------------------|--------------------|--------|
| group, index, value | 23 | 3 |
| structure, value | 26 | 2 and a structure copy |
| `port_t`, value | 19 | 2 |

Group 0 is the default group, the one the BIOS leaves selected. For any other
group, `out_port` and `inp_port` select the group, do the input or output,
and select group 0 again, as the BIOS does. For group 0 they leave the
expander alone, so use group 0 on a system that has no expander.

The group is written to the expander port as it is, so it is the same number
the BIOS uses for the group.

## Notes

* An EF line is asserted when its pin is low, which is when the `B1` to `B4`
  instructions branch.
* The port index is part of the opcode of the 1802 input and output
  instructions, so `out_port`, `inp_port` and `set_exp_port` store the
  instruction they need into the library code before it runs. The program
  has to run from RAM, as programs for Elf/OS and ELF-DOS do, and these
  functions must not be called from an interrupt routine.
* Only the low three bits of the port index are used. Do not use index 0,
  there is no such port.
* An interrupt routine that does I/O of its own while another group is
  selected will use the wrong group.
* The delay functions read the clock frequency in kHz from memory locations
  `$3E` and `$3F`, high byte first, where EDOS-mbios and other BIOSes store
  it. If both are zero, `FREQ_KHZ` is used.
* The delay functions count the time it takes to call them from C as part of
  the delay, and carry the fraction of a loop that is left over into the next
  millisecond, so the error does not add up. A delay is within about 120
  clock cycles of the time asked for, which is 30 microseconds at 4 MHz.
  The shortest delay is the time the call itself takes, about half a
  millisecond at 4 MHz and one millisecond at 1.79 MHz.
* A delay is longer by the time spent in interrupts and DMA while it runs.
* The clock has to be 300 kHz or faster for the delay functions.
