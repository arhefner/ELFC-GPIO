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

# The linker looks for gpio.lib in the lib folder of ElfC and in the
# current folder, so a copy is made here to build without installing it
all: gpio.lib
	@for file in $(FILES); do \
		echo "Building: $$file"; \
		$(CC) $(CFLAGS) $$file; \
	done

gpio.lib: ../lib/gpio.lib
	cp $< $@

# A clean target to remove generated files, an ELF-DOS program
# has the name of its source file without an extension
clean:
	rm -f *.prg *.asm *.lst *.build *.lkb *.sym *.elfos gpio.lib
	rm -f $(FILES:.c=)
