CC := elfc
CFLAGS :=
FILES := $(wildcard *.c)

# The operating system to build for, given on the command line:
#   make                    build for Elf/OS
#   make TARGET_OS=elfdos   build for ELF-DOS
TARGET_OS ?= elfos

ifeq ($(TARGET_OS),elfdos)
CFLAGS += -E
else ifneq ($(TARGET_OS),elfos)
$(error TARGET_OS must be elfos or elfdos, not '$(TARGET_OS)')
endif

# The system to build for, which selects the ports in sysconfig.h:
#   make SYSTEM=SUPERELF
SYSTEM ?= 1802MAX
CFLAGS += -D SYS_$(SYSTEM)

# The i2c driver to link with, see gpio.h:
#   make I2C=BB    the bit-banged driver, i2c_bb.lib
I2C ?= BB
CFLAGS += -D I2C_$(I2C)

# The function that tests the input button, ef4_asserted if not given:
#   make BUTTON=ef1_asserted
ifdef BUTTON
CFLAGS += -D BUTTON_PRESSED=$(BUTTON)
endif

# The linker looks for gpio.lib in the lib folder of ElfC and in the
# current folder, so a copy is made here to build without installing it.
# The same goes for the i2c drivers, and for the display libraries
# in the led7 and ssd1305 folders.
DRIVERS := $(notdir $(wildcard ../lib/i2c_*.lib))

all: gpio.lib $(DRIVERS) led7.lib ssd1305.lib
	@for file in $(FILES); do \
		echo "Building: $$file"; \
		$(CC) $(CFLAGS) $$file; \
	done

gpio.lib: ../lib/gpio.lib
	cp $< $@

$(DRIVERS): %.lib: ../lib/%.lib
	cp $< $@

led7.lib: led7/led7.lib
	cp $< $@

led7/led7.lib: led7/*.c led7/*.h ../include/gpio.h
	$(MAKE) -C led7

ssd1305.lib: ssd1305/ssd1305.lib
	cp $< $@

ssd1305/ssd1305.lib: ssd1305/*.c ssd1305/*.h ../include/gpio.h
	$(MAKE) -C ssd1305

# A clean target to remove generated files, an ELF-DOS program
# has the name of its source file without an extension
clean:
	rm -f *.prg *.asm *.lst *.build *.lkb *.sym *.elfos gpio.lib i2c_*.lib led7.lib ssd1305.lib
	rm -f $(FILES:.c=)
	$(MAKE) -C led7 clean
	$(MAKE) -C ssd1305 clean
