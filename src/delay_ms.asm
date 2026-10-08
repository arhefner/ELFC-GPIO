#include    gpio.inc

; void delay_ms(unsigned int ms)
;
; Waits for a number of milliseconds, 0 to 65535.
;
; A millisecond is as many clock cycles as the clock frequency in khz,
; which the bios keeps at clkfreq. RC holds the number of clock cycles
; that are still to be waited for. Every millisecond the frequency is
; added to it, less the cycles that the loop itself takes, and then
; WAITCLK is taken off for every pass of the wait loop until it goes
; negative. What is left over is carried into the next millisecond, so
; the error does not add up, and the delay as a whole is never off by
; more than one pass of the wait loop.
;
; RC starts out negative by the cycles it takes to call this procedure
; from C and to get to the loop and back, so that these are part of
; the delay as well. If the clock is too slow for that, the wait loop
; is skipped until RC is positive.
;
; All instructions take 2 machine cycles of 8 clock cycles, except for
; the long branches, which take 3. The numbers after the instructions
; in the loop are machine cycles. The long branches have to stay long
; for the timing to be right, so the linker may not make them short.

.norelax

#define WAITCLK   120                   ; one pass of wait, 15 machine cycles
#define TICKCLK   264                   ; rest of loop, 33 machine cycles
#define CALLCLK   1648                  ; call from c, entry and exit,
                                        ;  about 206 machine cycles

            proc  Cdelay_ms

            inc   r7                    ; get the number of milliseconds
            lda   r7
            plo   rf
            ldn   r7
            phi   rf
            dec   r7                    ; put r7 back as it was
            dec   r7

            ldi   clkfreq.1             ; get the clock frequency
            phi   rd
            ldi   clkfreq.0
            plo   rd
            lda   rd
            phi   rc
            ldn   rd
            plo   rc

            lbnz  hasfreq               ; use the default if not set
            ghi   rc
            lbnz  hasfreq
            ldi   FREQ_KHZ.1
            phi   rc
            ldi   FREQ_KHZ.0
            plo   rc

hasfreq:    glo   rc                    ; clock cycles to add each time
            smi   TICKCLK.0
            plo   rd
            ghi   rc
            smbi  TICKCLK.1
            phi   rd

            ldi   (0-CALLCLK).0         ; clock cycles used by the call
            plo   rc
            ldi   (0-CALLCLK).1
            phi   rc

loop:       glo   rf                    ; 2  until count is zero
            lbnz  tick                  ; 3
            ghi   rf
            lbz   return

tick:       dec   rf                    ; 2

            glo   rd                    ; 20 add cycles for a millisecond
            str   r2
            glo   rc
            add
            plo   rc
            ghi   rd
            str   r2
            ghi   rc
            adc
            phi   rc
            lbnf  loop                  ; 3  no wait if still negative

wait:       glo   rc                    ; 12 take off cycles for this pass
            smi   WAITCLK
            plo   rc
            ghi   rc
            smbi  0
            phi   rc
            lbdf  wait                  ; 3  until it goes negative

            lbr   loop                  ; 3

return:     sep   sret

            endp
