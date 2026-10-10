#include    gpio.inc

; void spi_init(port_t ctrl, port_t data)
; void spi_control(byte lines)
; void spi_write(const byte *data, unsigned int len)
; void spi_read(byte *data, unsigned int len)
; void spi_transfer(const byte *wdata, byte *rdata, unsigned int len)
; void spi_write_dma(const byte *data, unsigned int len)
; void spi_read_dma(byte *data, unsigned int len)
;
; The SPI functions, for the SPI adapter of the 1802/Mini and the
; 1802/MAX, which sends and receives a byte with a shift register:
;
;   https://github.com/arhefner/1802-Mini-SPI-DMA
;
; The adapter has a control port and a data port. A byte written to
; the data port is sent, and the byte that came in meanwhile is read
; from the data port afterwards. These are the bits of the control
; port:
;
;   bit 7     if clear, bits 0 to 6 are the lines and modes below,
;             if set, they are the number of bytes for DMA
;   bit 6     DMA out, the bytes at R0 are sent
;   bit 5     DMA in
;   bit 4     makes the number of bytes for DMA 128 more
;   bit 3, 2  chip select 1 and 0
;   bit 1, 0  general outputs 1 and 0
;
; spi_control sets bits 0 to 3. It keeps them in ctlval, as the
; control port is written with them again to start and end DMA.
;
; spi_write, spi_read and spi_transfer use the data port for every
; byte. spi_write_dma and spi_read_dma use DMA, which takes one
; machine cycle for a byte. DMA starts when DMA out or DMA in is set
; and the number of bytes is written, and the processor does nothing
; else until all are done. The bytes are done 64 at a time, as the
; bios does, so that interrupts are not held up for long, and then
; the ones that are left over.
;
; For DMA in, the adapter sends $FF by itself to get the next byte
; whenever one is stored. One byte has to be sent first to get the
; first one, as the bios does, so one more byte is clocked out of the
; device than is stored.
;
; The port index is part of the opcode of the input and output
; instructions, so spi_init stores the instructions for the two ports
; into all the places that use them, from the lists at the end.
;
; The group is only selected, and set back to 0 afterwards, if it is
; not 0. A load instruction is in the place of the output instruction
; that selects the group, and takes the byte after it just the same.
; For another group, the output instruction of out_port is copied to
; that place, so that the port that set_exp_port has set is used.
;
; The linker takes a procedure from the library when the name of the
; procedure is used, not for a label inside it. So this procedure
; names out_port, and the other six functions are procedures at the
; end of this file that name this one and branch to their label.

#define SPI_DMAIN   20h                 ; dma in mode
#define SPI_DMAOUT  40h                 ; dma out mode
#define SPI_COUNT   80h                 ; set the number of bytes for dma
#define SPI_BURST   64                  ; bytes for one dma operation

            extrn gpio_osel

            proc  Cspi_init

.link .requires Cout_port

            public spi_control
            public spi_write
            public spi_read
            public spi_transfer
            public spi_write_dma
            public spi_read_dma

            inc   r7                    ; get the index of control port
            lda   r7
            ani   7                     ; make its output instruction
            ori   60h
            plo   re
            lda   r7                    ; get the group
            plo   ra
            ldn   r7                    ; get the index of data port
            ani   7
            phi   ra
            dec   r7                    ; put r7 back as it was
            dec   r7
            dec   r7

            ldi   sites.1               ; point to the lists
            phi   rf
            ldi   sites.0
            plo   rf

            ldi   setsites.1            ; and to the subroutine
            phi   rd
            ldi   setsites.0
            plo   rd

            sep   rd                    ; outputs to the control port

            ghi   ra                    ; outputs to the data port
            ori   60h
            plo   re
            sep   rd

            ghi   ra                    ; inputs from the data port
            ori   68h
            plo   re
            sep   rd

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

            ldi   ctlval.1              ; no chip is selected
            phi   rf
            ldi   ctlval.0
            plo   rf
            ldi   3
            str   rf

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

          ; void spi_control(byte lines)
          ;
          ; Sets the chip selects and the general outputs.

spi_control: inc  r7                    ; get the lines
            ldn   r7
            dec   r7
            ani   0fh
            plo   re

            ldi   ctlval.1              ; keep them for spi_write
            phi   rf
            ldi   ctlval.0
            plo   rf
            glo   re
            str   rf

            sex   r3                    ; select the group
csel:       ldi   0

            sex   rf                    ; write the control port
cout:       out   1

            sex   r3                    ; back to the default group
crst:       ldi   0

            sex   r2
            sep   sret

ctlval:     db    3

          ; void spi_write(const byte *data, unsigned int len)
          ;
          ; Sends len bytes.

spi_write:  inc   r7                    ; get the pointer
            lda   r7
            plo   rf
            lda   r7
            phi   rf
            lda   r7                    ; get the length
            plo   rc
            ldn   r7
            phi   rc
            dec   r7                    ; put r7 back as it was
            dec   r7
            dec   r7
            dec   r7

            sex   r3                    ; select the group
bsel:       ldi   0

            sex   rf                    ; send from the pointer

bnext:      glo   rc                    ; until all are sent
            lbnz  bbyte
            ghi   rc
            lbz   bend

bbyte:
bout:       out   1
            dec   rc
            lbr   bnext

bend:       sex   r3                    ; back to the default group
brst:       ldi   0

            sex   r2
            sep   sret

          ; void spi_write_dma(const byte *data, unsigned int len)
          ; void spi_read_dma(byte *data, unsigned int len)
          ;
          ; Sends or receives len bytes with DMA. The two are the same
          ; but for the mode, which is kept on the stack and then in
          ; RA.0. RC is the number of whole DMA operations and RE.0 the
          ; number of bytes that are left over, RA.1 is the value of the
          ; control port.

spi_read_dma: ldi SPI_DMAIN
            lskp
spi_write_dma: ldi SPI_DMAOUT
            stxd

            inc   r7                    ; dma uses r0 as the pointer
            lda   r7
            plo   r0
            lda   r7
            phi   r0
            lda   r7                    ; get the length
            plo   rc
            ldn   r7
            phi   rc
            dec   r7                    ; put r7 back as it was
            dec   r7
            dec   r7
            dec   r7

            glo   rc                    ; bytes that are left over
            ani   SPI_BURST-1
            plo   re

            ldi   0                     ; divide the length by 64, which
            plo   ra                    ;  is the high two bytes of the
            glo   rc                    ;  length times 4
            shl
            plo   rc
            ghi   rc
            shlc
            phi   rc
            glo   ra
            shlc
            plo   ra
            glo   rc
            shl
            ghi   rc
            shlc
            plo   rc
            glo   ra
            shlc
            phi   rc

            irx                         ; get the mode
            ldx
            plo   ra

            ldi   ctlval.1              ; get the control port value
            phi   rf
            ldi   ctlval.0
            plo   rf
            ldn   rf
            phi   ra

            sex   r3                    ; select the group
wsel:       ldi   0

wout1:      out   1                     ; no bytes, so dma does not start
            db    SPI_COUNT

            sex   r2                    ; dma mode
            glo   ra
            str   r2
            ghi   ra
            or
            str   r2
wout2:      out   1
            dec   r2

            sex   r3

            glo   ra                    ; to receive, send a byte first
            xri   SPI_DMAIN             ;  to get the first one
            lbnz  wnext
pout:       out   1
            db    0ffh

wnext:      glo   rc                    ; until all whole operations done
            lbnz  wburst
            ghi   rc
            lbz   wrest

wburst:
wout3:      out   1                     ; 64 bytes
            db    SPI_COUNT+SPI_BURST
            dec   rc
            lbr   wnext

wrest:      glo   re                    ; the bytes that are left
            lbz   wend
            ori   SPI_COUNT
            str   r2
            sex   r2
wout4:      out   1
            dec   r2

wend:       ghi   ra                    ; end dma mode
            str   r2
            sex   r2
wout5:      out   1
            dec   r2

            sex   r3                    ; back to the default group
wrst:       ldi   0

            sex   r2
            sep   sret

          ; void spi_read(byte *data, unsigned int len)
          ;
          ; Receives len bytes, sending $FF for each of them.

spi_read:   inc   r7                    ; get the pointer
            lda   r7
            plo   rf
            lda   r7
            phi   rf
            lda   r7                    ; get the length
            plo   rc
            ldn   r7
            phi   rc
            dec   r7                    ; put r7 back as it was
            dec   r7
            dec   r7
            dec   r7

            sex   r3                    ; select the group
rsel:       ldi   0

rnext:      glo   rc                    ; until all are received
            lbnz  rbyte
            ghi   rc
            lbz   rend

rbyte:      sex   r3                    ; send $ff
rout:       out   1
            db    0ffh
            sex   rf                    ; store the byte that came in
rinp:       inp   1
            inc   rf
            dec   rc
            lbr   rnext

rend:       sex   r3                    ; back to the default group
rrst:       ldi   0

            sex   r2
            sep   sret

          ; void spi_transfer(const byte *wdata, byte *rdata,
          ;                   unsigned int len)
          ;
          ; Sends len bytes and stores the byte that came in with each of
          ; them. The byte is sent before the one that came in is stored,
          ; so the two pointers may be the same.

spi_transfer: inc r7                    ; get the pointer to send from
            lda   r7
            plo   rd
            lda   r7
            phi   rd
            lda   r7                    ; get the pointer to receive to
            plo   rf
            lda   r7
            phi   rf
            lda   r7                    ; get the length
            plo   rc
            ldn   r7
            phi   rc
            dec   r7                    ; put r7 back as it was
            dec   r7
            dec   r7
            dec   r7
            dec   r7
            dec   r7

            sex   r3                    ; select the group
tsel:       ldi   0

tnext:      glo   rc                    ; until all are done
            lbnz  tbyte
            ghi   rc
            lbz   tend

tbyte:      sex   rd                    ; send the next byte
tout:       out   1
            sex   rf                    ; store the byte that came in
tinp:       inp   1
            inc   rf
            dec   rc
            lbr   tnext

tend:       sex   r3                    ; back to the default group
trst:       ldi   0

            sex   r2
            sep   sret

          ; The places to change, a list for each kind, in the order
          ; that spi_init goes through them. A list ends with a zero,
          ; which the high byte of an address is not.

sites:      dw    cout, wout1, wout2, wout3, wout4, wout5
            db    0
            dw    bout, pout, rout, tout
            db    0
            dw    rinp, tinp
            db    0
            dw    csel, crst, bsel, brst, wsel, wrst, rsel, rrst, tsel, trst
            db    0
            dw    csel+1, bsel+1, wsel+1, rsel+1, tsel+1
            db    0

            endp

            proc  Cspi_control
.link .requires Cspi_init
            extrn spi_control
            lbr   spi_control
            endp

            proc  Cspi_write
.link .requires Cspi_init
            extrn spi_write
            lbr   spi_write
            endp

            proc  Cspi_read
.link .requires Cspi_init
            extrn spi_read
            lbr   spi_read
            endp

            proc  Cspi_transfer
.link .requires Cspi_init
            extrn spi_transfer
            lbr   spi_transfer
            endp

            proc  Cspi_write_dma
.link .requires Cspi_init
            extrn spi_write_dma
            lbr   spi_write_dma
            endp

            proc  Cspi_read_dma
.link .requires Cspi_init
            extrn spi_read_dma
            lbr   spi_read_dma
            endp
