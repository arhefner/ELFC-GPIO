#include    gpio.inc

; void tick_start(port_t rtc)
; void tick_stop(void)
; byte ticks(void)
; void tick_wait(byte n)
;
; The tick functions, which count the interrupts of the real time
; clock of the 1802/Mini and the 1802/MAX, an RTC-72421 that is set
; here to interrupt 64 times in a second.
;
; A byte written to the port of the clock is one of these:
;
;   2x    select register x of the clock
;   1x    write x to the register that is selected
;   40    do not let the interrupt of the clock through to the processor
;   41    let it through
;
; Register E is set to 2, which is an interrupt 64 times in a second
; that stays until bit 2 of register D is cleared, and to 0 again at
; the end, which is how the bios leaves the clock. The interrupt
; routine clears register D.
;
; The port index is part of the opcode of an output instruction, so
; tick_start stores the instruction for the port into all the places
; that write to it, from the lists at the end.
;
; The group is only selected, and set back to 0 afterwards, if it is
; not 0. A load instruction is in the place of the output instruction
; that selects the group, and takes the byte after it just the same.
; For another group, the output instruction of out_port is copied to
; that place, so that the port that set_exp_port has set is used.
;
; The linker takes a procedure from the library when the name of the
; procedure is used, not for a label inside it. So this procedure
; names out_port, and the other three functions are procedures at the
; end of this file that name this one and branch to their label.

            extrn gpio_osel

            proc  Ctick_start

.link .requires Cout_port

            public tick_stop
            public ticks
            public tick_wait
            public tick_count

            inc   r7                    ; get the port index
            lda   r7
            ani   7                     ; make the output instruction
            ori   60h
            plo   re
            ldn   r7                    ; get the group
            dec   r7                    ; put r7 back as it was
            dec   r7
            plo   ra

            ldi   sites.1               ; point to the lists
            phi   rf
            ldi   sites.0
            plo   rf

            ldi   setsites.1            ; and to the subroutine
            phi   rd
            ldi   setsites.0
            plo   rd

            sep   rd                    ; outputs to the clock

            glo   ra                    ; no select if group is zero
            lbz   nogroup

            ldi   gpio_osel.1           ; output instruction of out_port
            phi   rc
            ldi   gpio_osel.0
            plo   rc
            ldn   rc
            lskp

nogroup:    ldi   0f8h                  ; a load instruction
            plo   re
            sep   rd                    ; group selects and resets

            glo   ra                    ; the group after the selects
            plo   re
            sep   rd

            ldi   running.1             ; keep r1 for tick_stop, unless
            phi   rf                    ;  it is the routine already
            ldi   running.0
            plo   rf
            ldn   rf
            lbnz  restart

            ldi   1
            str   rf
            inc   rf
            ghi   r1
            str   rf
            inc   rf
            glo   r1
            str   rf

restart:    ldi   tick_count.1          ; no ticks yet, none waited for
            phi   rf
            ldi   tick_count.0
            plo   rf
            ldi   0
            str   rf
            inc   rf
            str   rf

            ldi   introu.1              ; r1 is the interrupt routine
            phi   r1
            ldi   introu.0
            plo   r1

            ldi   rtcsend.1             ; set the clock up
            phi   rd
            ldi   rtcsend.0
            plo   rd
            sep   rd
            db    2eh, 12h              ; register e, interrupt at 64 hz
            db    41h                   ; let the interrupt through
            db    0

            sex   r3                    ; enable interrupts, x is 2 again
            ret
            db    23h

            sep   sret

          ; Store RE.0 into the places of the list at RF, up to the zero
          ; at its end, and leave RF at the next list. Called with RD as
          ; the program counter, which is left here for the next call.

setret:     sep   r3

setsites:   lda   rf                    ; until the end of the list
            lbz   setret
            phi   rc
            lda   rf
            plo   rc
            glo   re
            str   rc
            lbr   setsites

          ; Write the bytes after the call to the port of the clock, up to
          ; a zero. Called with RD as the program counter. X is R3 for the
          ; bytes, and RD for the byte after the group select.

sendret:    sep   r3

rtcsend:    sex   rd                    ; select the group
ssel:       ldi   0
            sex   r3

sendnext:   ldn   r3                    ; until the zero
            lbz   sendend
sout:       out   1
            lbr   sendnext

sendend:    inc   r3                    ; skip the zero

            sex   rd                    ; back to the default group
srst:       ldi   0
            sex   r2
            lbr   sendret

          ; void tick_stop(void)
          ;
          ; Stops the ticks and leaves the clock as the bios had it, with
          ; interrupts disabled and R1 as it was.

tick_stop:  ldi   running.1             ; nothing to do if not started
            phi   rf
            ldi   running.0
            plo   rf
            ldn   rf
            lbz   stopped

            sex   r3                    ; disable interrupts, x is 2 again
            dis
            db    23h

            ldi   0                     ; not running, get r1 back
            str   rf
            inc   rf
            lda   rf
            phi   r1
            ldn   rf
            plo   r1

            ldi   rtcsend.1
            phi   rd
            ldi   rtcsend.0
            plo   rd
            sep   rd
            db    40h                   ; do not let the interrupt through
            db    2eh, 10h              ; register e, pulses at 64 hz
            db    2dh, 10h              ; register d, clear the interrupt
            db    0

stopped:    sep   sret

          ; byte ticks(void)
          ;
          ; Returns the number of ticks, which starts over after 255.

ticks:      ldi   tick_count.1
            phi   rf
            ldi   tick_count.0
            plo   rf
            ldn   rf
            plo   ra
            ldi   0
            phi   ra
            sep   sret

          ; void tick_wait(byte n)
          ;
          ; Waits until n ticks after the time the wait before this one
          ; ended, or after tick_start for the first. If that time is
          ; past, it does not wait, so a caller that is late once is on
          ; time again the next time. If twice that time is past, the
          ; caller can not keep up, and the time is counted from now on.
          ;
          ; RF points to the count and RC to the tick at which the last
          ; wait ended, RA.0 is n and RE.0 the ticks since then.

tick_wait:  inc   r7                    ; get the number of ticks
            ldn   r7
            dec   r7
            plo   ra

            ldi   running.1             ; do not wait if not started
            phi   rf
            ldi   running.0
            plo   rf
            ldn   rf
            lbz   waited

            ldi   tick_count.1
            phi   rf
            ldi   tick_count.0
            plo   rf
            ldi   last.1
            phi   rc
            ldi   last.0
            plo   rc

waiting:    sex   rc                    ; ticks since the last wait
            ldn   rf
            sm
            plo   re
            sex   r2                    ; until that is n or more
            glo   ra
            str   r2
            glo   re
            sm
            lbnf  waiting

            glo   ra                    ; twice n, if that is a byte
            shl
            lbdf  ontime
            str   r2
            glo   re
            sm
            lbnf  ontime

            glo   re                    ; too late, count from now on
            lbr   setlast

ontime:     glo   ra                    ; the time this wait was for

setlast:    sex   rc
            add
            str   rc
            sex   r2

waited:     sep   sret

          ; The interrupt routine. It clears the interrupt of the clock
          ; and adds one to the count. R1 is its program counter, and X
          ; as well for the bytes after the output instructions.
          ;
          ; It ends by branching to the RET before its first instruction,
          ; which leaves R1 at that instruction for the next interrupt.

intret:     ret

introu:     dec   r2                    ; save x and p, d and df
            sav
            dec   r2
            stxd
            shrc
            stxd
            ghi   rf                    ; and rf
            stxd
            glo   rf
            stxd

            sex   r1
isel:       ldi   0                     ; select the group
iout1:      out   1                     ; select register d
            db    2dh
iout2:      out   1                     ; clear the interrupt
            db    10h
irst:       ldi   0                     ; back to the default group
            sex   r2

            ldi   tick_count.1          ; add one to the count
            phi   rf
            ldi   tick_count.0
            plo   rf
            ldn   rf
            adi   1
            str   rf

            irx                         ; get back what was saved
            ldxa
            plo   rf
            ldxa
            phi   rf
            ldxa
            shl
            ldxa
            lbr   intret

running:    db    0                     ; not zero after tick_start
            db    0, 0                  ; r1 as it was before
tick_count: db    0                     ; the ticks
last:       db    0                     ; tick at which the last wait ended

          ; The places to change, a list for each kind, in the order
          ; that tick_start goes through them. A list ends with a zero,
          ; which the high byte of an address is not.

sites:      dw    sout, iout1, iout2
            db    0
            dw    ssel, srst, isel, irst
            db    0
            dw    ssel+1, isel+1
            db    0

            endp

            proc  Ctick_stop
.link .requires Ctick_start
            extrn tick_stop
            lbr   tick_stop
            endp

            proc  Cticks
.link .requires Ctick_start
            extrn ticks
            lbr   ticks
            endp

            proc  Ctick_wait
.link .requires Ctick_start
            extrn tick_wait
            lbr   tick_wait
            endp
