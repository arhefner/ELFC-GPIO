#include    gpio.inc

; void set_exp_port(byte port)
;
; Sets the port that selects the port group, 1 to 7, for systems where
; it is not the port the library was built for.
;
; The port is part of the opcode of the output instructions that select
; the group in out_port and inp_port, so those four instructions are
; replaced here. The output instruction is kept in RE.0 meanwhile.

            extrn gpio_osel
            extrn gpio_orst
            extrn gpio_isel
            extrn gpio_irst

            proc  Cset_exp_port

            inc   r7                    ; get the port
            ldn   r7
            dec   r7
            ani   7                     ; make the output instruction
            ori   60h
            plo   re

            ldi   gpio_osel.1           ; group select in out_port
            phi   rf
            ldi   gpio_osel.0
            plo   rf
            glo   re
            str   rf

            ldi   gpio_orst.1           ; group reset in out_port
            phi   rf
            ldi   gpio_orst.0
            plo   rf
            glo   re
            str   rf

            ldi   gpio_isel.1           ; group select in inp_port
            phi   rf
            ldi   gpio_isel.0
            plo   rf
            glo   re
            str   rf

            ldi   gpio_irst.1           ; group reset in inp_port
            phi   rf
            ldi   gpio_irst.0
            plo   rf
            glo   re
            str   rf

            sep   sret

            endp
