SHELL := /bin/bash
CC = elfc
CFLAGS = -L

SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.prg)
TARGET = led7.lib

all: $(TARGET)

$(TARGET): $(OBJS)
	cat $^ > $@

%.prg: %.c led7.h led7_int.h ../../include/gpio.h
	$(CC) $(CFLAGS) $<

clean:
	rm -f *.asm *.lst *.build $(OBJS) $(TARGET)
