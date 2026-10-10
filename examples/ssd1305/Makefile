SHELL := /bin/bash
CC = elfc
CFLAGS = -L

SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.prg)
TARGET = ssd1305.lib

all: $(TARGET)

$(TARGET): $(OBJS)
	cat $^ > $@

%.prg: %.c ssd1305.h ssd1305_int.h ../../include/gpio.h
	$(CC) $(CFLAGS) $<

clean:
	rm -f *.asm *.lst *.build $(OBJS) $(TARGET)
