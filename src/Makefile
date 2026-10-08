SHELL := /bin/bash
ASM = asm02
ASMFLAGS = -q -L

# The port that selects the port group on the system the library is
# built for, a program can change it with set_exp_port:
#   make               1802/Mini, 1802/MAX and Super Elf
#   make EXP_PORT=1    most other systems
EXP_PORT ?= 5

# The clock frequency in kHz that the delay functions assume if the
# BIOS has not stored the real one
FREQ_KHZ ?= 4000

ASMFLAGS += -DEXP_PORT=$(EXP_PORT) -DFREQ_KHZ=$(FREQ_KHZ)

SRCS = $(wildcard *.asm)
OBJS = $(SRCS:.asm=.prg)
TARGET = gpio.lib

all: $(TARGET)

$(TARGET): $(OBJS)
	mkdir -p ../lib
	cat $^ > ../lib/$@

%.prg: %.asm gpio.inc
	$(ASM) $(ASMFLAGS) $<

clean:
	rm -f *.lst *.build $(OBJS) ../lib/$(TARGET)
