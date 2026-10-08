; gpio.inc - definitions shared by the procedures of the gpio library
;
; The procedures are written in assembly instead of C with inline
; assembly, so they have no stack frame and take their arguments
; straight from the expression stack of ElfC:
;
;   R7 points one byte below the first argument, every argument is
;   two bytes with the low byte at the lower address, so the first
;   argument is at R7+1 and R7+2 and the second at R7+3 and R7+4.
;
;   The result is returned in RA.
;
;   R7, R9 and RB are preserved, RC, RD, RF and RE.0 are free to use.
;
; No subroutine of the ElfC runtime is called, so the same library
; works in programs for Elf/OS and for ELF-DOS.

scall:      equ   r4                    ; standard call
sret:       equ   r5                    ; standard return

clkfreq:    equ   003eh                 ; processor clock frequency in khz,
                                        ;  high byte first, set by the bios

          ; The port that selects the group on a system with a port group
          ; expander. It is 5 on the 1802/Mini, 1802/MAX and Super Elf and
          ; 1 on most other systems. The makefile sets it, and a program
          ; changes it when it runs with set_exp_port.

#ifndef EXP_PORT
#define EXP_PORT 5
#endif

          ; The clock frequency in khz that the delay routines assume when
          ; the bios has not set clkfreq, the same as mbios assumes.

#ifndef FREQ_KHZ
#define FREQ_KHZ 4000
#endif
