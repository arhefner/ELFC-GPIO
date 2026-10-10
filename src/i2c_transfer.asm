#include    gpio.inc
#include    i2c.inc

; bool i2c_transfer(byte addr, const byte *wdata, unsigned int wlen,
;                   byte *rdata, unsigned int rlen)
; bool i2c_write(byte addr, const byte *data, unsigned int len)
; bool i2c_read(byte addr, byte *data, unsigned int len)
; bool i2c_clear(void)
; bool i2c_init(port_t port)
;
; The i2c functions. They do not touch the hardware, that is left to
; the i2c driver the program is linked with. See i2c.inc for what a
; driver is and how it is called.
;
; A message is a write part, a read part, or a write part and then a
; read part with a repeated start between them, as is used to read the
; registers of a device. The three functions that send a message are
; the same code, entered with a different write part and read part:
;
;   i2c_transfer   both parts are arguments
;   i2c_write      the read part has no bytes
;   i2c_read       the write part has no bytes
;
; A part that has no bytes is left out. If both have none, only the
; address is sent, which shows if there is a device at that address.
;
; The pointer and the length of the read part are kept on the stack
; while the write part is sent.
;
; The result is true if the device acknowledged its address and every
; byte that was sent to it.
;
; The linker takes a procedure from the library when the name of the
; procedure is used, not for a label inside it. So this procedure
; names the driver, and the other four functions are procedures at
; the end of this file that name this one and branch to their label.

            extrn i2c_dopen
            extrn i2c_dstart
            extrn i2c_dwrite
            extrn i2c_dread
            extrn i2c_dstop
            extrn i2c_dclear

            proc  Ci2c_transfer

.link .requires i2c_driver

            public i2c_write
            public i2c_read
            public i2c_clear

            ldi   10                    ; read part ends at argument byte 10
            lskp
i2c_read:   ldi   6                     ; read part ends at argument byte 6
            plo   re

            str   r2                    ; point to end of the read part
            glo   r7
            add
            plo   rf
            ghi   r7
            adci  0
            phi   rf

            ldn   rf                    ; push length and pointer
            stxd
            dec   rf
            ldn   rf
            stxd
            dec   rf
            ldn   rf
            stxd
            dec   rf
            ldn   rf
            stxd
            dec   rf

            glo   re                    ; write part is before the read part
            smi   10
            lbz   getwrite

            ldi   0                     ; i2c_read has no write part
            plo   rc
            phi   rc
            lbr   begin

i2c_write:  ldi   0                     ; push a read part with no bytes
            stxd
            stxd
            stxd
            stxd

            glo   r7                    ; point to end of the write part
            adi   6
            plo   rf
            ghi   r7
            adci  0
            phi   rf

getwrite:   ldn   rf                    ; get the length
            phi   rc
            dec   rf
            ldn   rf
            plo   rc
            dec   rf
            ldn   rf                    ; get the pointer
            plo   ra
            dec   rf
            ldn   rf
            plo   rf
            glo   ra
            phi   rf

begin:      ldi   i2c_dopen.1           ; page of the driver subroutines
            phi   rd
            ldi   i2c_dopen.0
            plo   rd
            sep   rd

            glo   rc                    ; skip write part if no bytes
            lbnz  wstart
            ghi   rc
            lbnz  wstart

            ldi   1                     ; nothing is sent yet
            lbr   rpart

wstart:     ldi   i2c_dstart.0          ; start and address to write
            plo   rd
            adi   0
            sep   rd
            lbdf  nodev

wnext:      lda   rf                    ; send the next byte
            plo   ra
            ldi   i2c_dwrite.0
            plo   rd
            sep   rd
            lbdf  nodev

            dec   rc                    ; until all are sent
            glo   rc
            lbnz  wnext
            ghi   rc
            lbnz  wnext

            ldi   0                     ; the address is sent

rpart:      plo   ra                    ; keep if nothing is sent yet

            irx                         ; get the read part
            ldxa
            plo   rf
            ldxa
            phi   rf
            ldxa
            plo   rc
            ldx
            phi   rc

            glo   rc                    ; skip read part if no bytes
            lbnz  rstart
            ghi   rc
            lbnz  rstart

            glo   ra                    ; done if the write part was sent
            lbz   good

            ldi   i2c_dstart.0          ; else send just the address
            plo   rd
            adi   0
            sep   rd
            lbr   stop

rstart:     ldi   i2c_dstart.0          ; start and address to read
            plo   rd
            smi   0
            sep   rd
            lbdf  stop

rnext:      dec   rc                    ; acknowledge all but the last
            glo   rc
            str   r2
            ghi   rc
            or
            sdi   0
            ldi   i2c_dread.0
            plo   rd
            sep   rd

            glo   ra                    ; store the byte
            str   rf
            inc   rf

            glo   rc                    ; until all are received
            lbnz  rnext
            ghi   rc
            lbnz  rnext
            lbr   good

          ; Free the bus. The driver returns DF set if the bus is free,
          ; the opposite of a message that was acknowledged.

i2c_clear:  ldi   i2c_dopen.1
            phi   rd
            ldi   i2c_dopen.0
            plo   rd
            sep   rd

            ldi   i2c_dclear.0
            plo   rd
            sep   rd

            ldi   0                     ; turn df around
            shlc
            xri   1
            shr
            lbr   stop

nodev:      irx                         ; drop the read part
            irx
            irx
            irx
            lskp                        ; df is still set

good:       adi   0                     ; all bytes were acknowledged

stop:       ldi   i2c_dstop.0           ; stop, df is not changed
            plo   rd
            sep   rd

            ldi   0                     ; result is true if df is clear
            phi   ra
            lsdf
            ldi   1
            plo   ra
            sep   sret

            endp

            proc  Ci2c_write
.link .requires Ci2c_transfer
            extrn i2c_write
            lbr   i2c_write
            endp

            proc  Ci2c_read
.link .requires Ci2c_transfer
            extrn i2c_read
            lbr   i2c_read
            endp

            proc  Ci2c_clear
.link .requires Ci2c_transfer
            extrn i2c_clear
            lbr   i2c_clear
            endp

            proc  Ci2c_init
.link .requires Ci2c_transfer
            extrn i2c_dinit
            lbr   i2c_dinit
            endp
