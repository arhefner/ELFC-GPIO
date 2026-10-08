#include    gpio.inc

; void delay_s(unsigned int s)
;
; Waits for a number of seconds, 0 to 65535, by calling delay_ms with
; 1000 milliseconds for every second.

            extrn Cdelay_ms

            proc  Cdelay_s

            inc   r7                    ; get the number of seconds
            lda   r7
            plo   rf
            ldn   r7
            phi   rf
            dec   r7                    ; put r7 back as it was
            dec   r7

loop:       glo   rf                    ; until count is zero
            lbnz  second
            ghi   rf
            lbz   return

second:     dec   rf

            ghi   rf                    ; delay_ms uses rf
            stxd
            glo   rf
            stxd

            sex   r7                    ; push 1000 as the argument
            ldi   1000.1
            stxd
            ldi   1000.0
            stxd
            sex   r2

            sep   scall                 ; wait for a second
            dw    Cdelay_ms

            inc   r7                    ; drop the argument
            inc   r7

            irx                         ; get the count back
            ldxa
            plo   rf
            ldx
            phi   rf

            lbr   loop

return:     sep   sret

            endp
