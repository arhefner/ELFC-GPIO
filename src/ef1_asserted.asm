#include    gpio.inc

; bool ef1_asserted(void)
;
; Returns true if the EF1 line is asserted, which is when the pin is low.
;
; The EF lines can only be tested with a short branch, which has to be
; on the same page as its target. The procedure is seven bytes, so it
; is aligned to eight bytes to keep it from crossing a page boundary.

.link .align qword

            proc  Cef1_asserted

            ldi   0                     ; result is false
            phi   ra
            plo   ra
            bn1   return                ; unless the line is asserted
            inc   ra

return:     sep   sret

            endp
