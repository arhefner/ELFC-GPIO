#include    ../src/gpio.inc
#include    ../src/i2c.inc

; i2c_bb - the bit-banged i2c driver, as in the ELF-I2C library
;
; The i2c bus is two bits of an output port and two EF lines. Each
; output bit drives an open collector or open drain driver, so that
; the line is pulled low when the bit is high, and is left to its
; pull-up resistor when the bit is low. The EF lines read the bus
; lines back, and are asserted when the line is low.
;
; The argument of i2c_init is the output port. The other six bits of
; the port are written as 0.
;
; See ../src/i2c.inc for what a driver is and how it is called. RA.1
; is the output port as it was last written, and RE.0 counts the bits.
;
; The port index is part of the opcode of the output instruction, so
; i2c_dinit stores the instruction for the port into all the places
; that write to it.
;
; The EF lines can only be tested with a short branch, which has to be
; on the same page as its target, and the list of the places to change
; holds the low byte of the addresses only. The procedure is aligned
; to a page to keep all of it on one page.

SDA_HIGH:   equ   07fh                  ; and with this to let sda go high
SDA_LOW:    equ   080h                  ; or with this to pull sda low
SCL_HIGH:   equ   0bfh                  ; and with this to let scl go high
SCL_LOW:    equ   040h                  ; or with this to pull scl low

#define BSDA_LOW  b3                    ; branch if sda is low
#define BSCL_LOW  b4                    ; branch if scl is low

            extrn gpio_osel
            extrn i2c_clear

.link .align page

            proc  i2c_driver

.link .requires Cout_port

            public i2c_dinit
            public i2c_dopen
            public i2c_dstart
            public i2c_dwrite
            public i2c_dread
            public i2c_dstop
            public i2c_dclear

          ; Set the port of the bus, then let go of both lines and free
          ; the bus, which i2c_clear does.
          ;
          ; The group is only selected, and set back to 0 afterwards, if
          ; it is not 0. A load instruction is in the place of the output
          ; instruction that selects the group, and takes the byte after
          ; it just the same. For another group, the output instruction
          ; of out_port is copied to that place, so that the port that
          ; set_exp_port has set is used.

i2c_dinit:  inc   r7                    ; get the port index
            lda   r7
            ani   7                     ; make the output instruction
            ori   60h
            plo   re
            ldn   r7                    ; get the group
            dec   r7                    ; put r7 back as it was
            dec   r7
            plo   ra

            ldi   sites.1               ; point to the list
            phi   rf
            phi   rc                    ; places are on the same page
            ldi   sites.0
            plo   rf

setnext:    lda   rf                    ; until the end of the list
            bz    setgroup
            plo   rc
            glo   re
            str   rc
            br    setnext

setgroup:   glo   ra                    ; no select if group is zero
            bz    nogroup

            ldi   gpio_osel.1           ; output instruction of out_port
            phi   rf
            ldi   gpio_osel.0
            plo   rf
            ldn   rf
            lskp

nogroup:    ldi   0f8h                  ; a load instruction
            plo   re

            ldi   selgrp.0              ; set the group select
            plo   rc
            glo   re
            str   rc
            inc   rc
            glo   ra                    ; the group goes after it
            str   rc

            ldi   rstgrp.0              ; same for the group reset
            plo   rc
            glo   re
            str   rc

            lbr   i2c_clear

sites:      db    out1.0, out2.0, out3.0, out4.0, out5.0
            db    out6.0, out7.0, out8.0, out9.0, out10.0
            db    0

          ; Select the group of the port. Both lines are high between
          ; messages, so the port was last written as zero. RD is the
          ; program counter here, so it is the one to point to the byte
          ; after the output instruction.

i2c_dopen:  sex   rd
selgrp:     ldi   0
            sex   r2

            ldi   0
            phi   ra
            br    return

          ; Make a start and send the address with DF as the read flag.
          ;
          ; The data line is let go and then the clock line, so this also
          ; makes a repeated start when the clock line is still low after
          ; another byte.

i2c_dstart: ghi   ra
            ani   SDA_HIGH              ; sda high
            str   r2
out1:       out   1
            dec   r2
            ani   SCL_HIGH              ; scl high
            str   r2
out2:       out   1
            dec   r2
swait:      BSCL_LOW swait              ; wait for scl to be high
            ori   SDA_LOW               ; sda low while scl is high
            str   r2
out3:       out   1
            dec   r2
            ori   SCL_LOW               ; scl low
            str   r2
out4:       out   1
            dec   r2
            phi   ra

            inc   r7                    ; get the address
            ldn   r7
            dec   r7
            shlc                        ; and add the read flag
            plo   ra
            br    i2c_dwrite

          ; Free the bus with nine clock pulses while the data line is
          ; let go, which is to receive a byte and not acknowledge it.
          ; The level of the data line at the last pulse is the result.

i2c_dclear: smi   0

          ; Receive a byte, which is to send $FF, so that the data line
          ; is let go, and then DF to acknowledge the byte or not.

i2c_dread:  ldi   0ffh
            plo   ra
            br    xbyte

          ; Send a byte, and then let go of the data line for the device
          ; to acknowledge it.

i2c_dwrite: smi   0

          ; Send the byte in RA.0 and then the bit in DF, which makes nine
          ; clock pulses. Returns the byte that was read meanwhile in RA.0,
          ; and the level of the data line at the ninth pulse in DF.
          ;
          ; Each pass of the loop shifts the bit to send out of RA.0 and
          ; the level that was read at the pulse before into it. The bit
          ; that came in with DF is shifted in first, so it is the ninth
          ; to be sent, and by then RA.0 is the eight bits that were read.
          ;
          ; A device may hold the clock line low to get more time, so the
          ; data line is not read until the clock line is high. A start
          ; and a stop wait for the clock line in the same way.

xbyte:      ldi   9
            plo   re

nextbit:    glo   ra                    ; get the bit to send
            shlc
            plo   ra

            ghi   ra
            bdf   one
            ori   SDA_LOW               ; sda low for a zero
            lskp
one:        ani   SDA_HIGH              ; sda high for a one
            str   r2
out5:       out   1
            dec   r2
            ani   SCL_HIGH              ; scl high
            str   r2
out6:       out   1
            dec   r2

stretch:    BSCL_LOW stretch            ; wait for scl to be high

            adi   0                     ; read sda into df
            BSDA_LOW zero
            smi   0

zero:       ori   SCL_LOW               ; scl low
            str   r2
out7:       out   1
            dec   r2
            phi   ra

            dec   re                    ; until all bits are done
            glo   re
            bnz   nextbit

return:     sep   r3

          ; Make a stop, which leaves both lines high, and go back to
          ; the default group.

i2c_dstop:  ghi   ra
            ori   SDA_LOW               ; sda low
            str   r2
out8:       out   1
            dec   r2
            ani   SCL_HIGH              ; scl high
            str   r2
out9:       out   1
            dec   r2
pwait:      BSCL_LOW pwait              ; wait for scl to be high
            ani   SDA_HIGH              ; sda high while scl is high
            str   r2
out10:      out   1
            dec   r2

            sex   rd
rstgrp:     ldi   0
            sex   r2
            br    return

            endp
