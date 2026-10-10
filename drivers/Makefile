SHELL := /bin/bash
ASM = asm02
ASMFLAGS = -q -L

# Every i2c driver is a library of its own, with the name of its
# source file. A program is linked with one of them, see gpio.h.
SRCS = $(wildcard *.asm)
OBJS = $(SRCS:.asm=.prg)
LIBS = $(SRCS:%.asm=../lib/%.lib)

all: $(LIBS)

../lib/%.lib: %.prg
	mkdir -p ../lib
	cp $< $@

%.prg: %.asm ../src/gpio.inc ../src/i2c.inc
	$(ASM) $(ASMFLAGS) $<

clean:
	rm -f *.lst *.build $(OBJS) $(LIBS)

.PRECIOUS: %.prg
