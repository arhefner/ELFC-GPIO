# The folder ElfC is installed in, for the install target
ELFC_DIR ?= /opt/elfc

.PHONY: all examples install clean

all:
	$(MAKE) -C src
	$(MAKE) -C drivers

examples: all
	$(MAKE) -C examples

# Put the libraries and the header file where ElfC looks for them
install: all
	cp lib/*.lib $(ELFC_DIR)/lib/
	cp include/gpio.h $(ELFC_DIR)/include/

clean:
	$(MAKE) -C src clean
	$(MAKE) -C drivers clean
	$(MAKE) -C examples clean
