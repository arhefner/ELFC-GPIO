; i2c.inc - the interface between the i2c functions and an i2c driver
;
; The i2c functions in i2c_transfer.asm are the same for all hardware.
; They take a message apart into starts, bytes and a stop, and call a
; driver for each of those. The driver is the only code that knows
; what the hardware is. Each driver is a library of its own in the
; drivers folder, and a program is linked with one of them.
;
; A driver is a procedure with the name i2c_driver, as the linker
; takes a procedure from a library by its name. It has these labels,
; all of them public:
;
;   i2c_dinit    The C function i2c_init branches here, with its
;                argument on the expression stack like any procedure
;                that is called from C. Sets up the hardware and then
;                branches to i2c_clear, which makes the result.
;
; The others are subroutines that are called with RD as the program
; counter, by setting RD to the subroutine and then SEP RD, and that
; come back with SEP R3. They have to be on one page, as RD.1 is set
; once for a message and only RD.0 for each call. X is R2 when they
; are called, and has to be when they come back.
;
;   i2c_dopen    Called first for every message, before the start.
;
;   i2c_dstart   Make a start, or a repeated start if there was one
;                in this message already, and send the address of the
;                device with the read flag. The address is the first
;                argument of the C function, the byte at R7+1, and the
;                read flag is DF. Returns DF set if no device
;                acknowledged the address.
;
;   i2c_dwrite   Send the byte in RA.0. Returns DF set if the device
;                did not acknowledge it.
;
;   i2c_dread    Receive a byte and return it in RA.0. DF is clear to
;                acknowledge the byte, or set if it is the last one of
;                the message.
;
;   i2c_dstop    Make a stop. Called last for every message, also when
;                the message ended early, so anything that i2c_dopen
;                did is undone here. DF is not changed.
;
;   i2c_dclear   Called between i2c_dopen and i2c_dstop in place of a
;                message. Frees the bus from a device that holds the
;                data line low. Returns DF set if the data line is
;                high, which is when the bus is free.
;
; The registers during a message:
;
;   RA.0  the byte that is sent or received
;   RA.1  for the driver, kept from one call to the next
;   RC    the number of bytes that are left, not to be changed
;   RD    program counter of the driver subroutines
;   RE.0  for the driver, not kept from one call to the next
;   RF    pointer to the next byte of the message, not to be changed
;
; i2c_dinit is not part of a message and is free to use RC and RF.
