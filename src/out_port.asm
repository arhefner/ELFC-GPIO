#include    gpio.inc

; void out_port(port_t port, byte value)
;
; Writes value to a port. The port holds the port group in its high
; byte and the port index, 1 to 7, in its low byte.
;
; The index is part of the opcode of the output instruction, so the
; instruction is made here and stored into the code before it is run.
; The bytes are sent straight from the expression stack by making R7
; the X register, which advances it the same way a pop would.
;
; Group 0 is the default group that the bios leaves selected, so the
; group is only selected, and set back to 0 afterwards, if it is not 0.
; This also means that a port in group 0 works without an expander.

            proc  Cout_port

            public gpio_osel
            public gpio_orst

            ldi   outins.1              ; point to output instruction
            phi   rf
            ldi   outins.0
            plo   rf

            sex   r7                    ; send from the expression stack
            irx                         ; get the port index
            ldxa
            ani   7                     ; make the output instruction
            ori   60h
            str   rf

            ldn   r7                    ; skip the select if group is zero
            lsz
gpio_osel:  out   EXP_PORT              ; select the group
            dec   r7
            inc   r7                    ; point to value either way

outins:     out   1                     ; send the value

            dec   r7                    ; get the group again
            dec   r7
            ldn   r7
            dec   r7                    ; put r7 back as it was
            dec   r7
            lbz   return                ; group was never changed if zero

            sex   r3                    ; back to the default group
gpio_orst:  out   EXP_PORT
            db    0

return:     sex   r2
            sep   sret

            endp
