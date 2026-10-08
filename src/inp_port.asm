#include    gpio.inc

; byte inp_port(port_t port)
;
; Returns the byte read from a port. The port holds the port group in
; its high byte and the port index, 1 to 7, in its low byte.
;
; The index is part of the opcode of the input instruction, so the
; instruction is made here and stored into the code before it is run.
; An input instruction also stores the byte at M(R(X)), which is made
; the low byte of the argument, since that is no longer needed.
;
; Group 0 is the default group that the bios leaves selected, so the
; group is only selected, and set back to 0 afterwards, if it is not 0.
; This also means that a port in group 0 works without an expander.

            proc  Cinp_port

            public gpio_isel
            public gpio_irst

            ldi   inpins.1              ; point to input instruction
            phi   rf
            ldi   inpins.0
            plo   rf

            sex   r7                    ; work on the expression stack
            irx                         ; get the port index
            ldxa
            ani   7                     ; make the input instruction
            ori   68h
            str   rf

            ldn   r7                    ; skip the select if group is zero
            lsz
gpio_isel:  out   EXP_PORT              ; select the group
            dec   r7
            dec   r7                    ; point to index either way

inpins:     inp   1                     ; read the port into the result
            plo   ra

            inc   r7                    ; get the group again
            ldn   r7
            dec   r7                    ; put r7 back as it was
            dec   r7
            lbz   return                ; group was never changed if zero

            sex   r3                    ; back to the default group
gpio_irst:  out   EXP_PORT
            db    0
            ldi   0

return:     phi   ra                    ; high byte of the result is zero
            sex   r2
            sep   sret

            endp
