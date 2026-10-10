#include "ssd1305_int.h"

/*
 * Make an image for the display from a Pixie image, which is the 64
 * by 32 image of the CDP1861 video chip, SSD1305_PIXIE_SIZE bytes
 * that are 32 rows of 8 bytes with the left pixel of a byte in bit 7.
 * Every pixel is made two wide and two high, so the image fills the
 * display.
 *
 * A byte of the display is eight pixels of a column, so four rows of
 * the Pixie image make a page.  The same bit of the four rows makes
 * a byte, the first row in the low two bits, and that byte is stored
 * twice.
 *
 * This is in assembly, as it takes a few seconds in C.  For each of
 * the 64 bytes of a row in all pages, the four bytes below each
 * other are taken into RD.0, RD.1, RC.0 and RC.1 and shifted left
 * together, one bit for every byte that is stored.  A bit is set
 * below the last bit of RD.0, so that RD.0 is zero when all eight
 * are done.  The number of bytes that are left is on the stack.
 */
void ssd1305_pixie(const byte *pixie, byte *frame) {
  asm("         gosub s_lget16    ; get the pointer to the image");
  asm("           dw  2           ; from arg 2");
  asm("         copy ra, rf");
  asm("         gosub s_lget16    ; get the pointer to the pixie image");
  asm("           dw  0           ; from arg 1");

  asm("         ldi  64           ; bytes of a row in all pages");
  asm("         stxd");

  asm("pix_col: ldn  ra           ; get the byte of the first row");
  asm("         plo  rd");
  asm("         glo  ra           ; and of the three rows below it");
  asm("         adi  8");
  asm("         plo  ra");
  asm("         ghi  ra");
  asm("         adci 0");
  asm("         phi  ra");
  asm("         ldn  ra");
  asm("         phi  rd");
  asm("         glo  ra");
  asm("         adi  8");
  asm("         plo  ra");
  asm("         ghi  ra");
  asm("         adci 0");
  asm("         phi  ra");
  asm("         ldn  ra");
  asm("         plo  rc");
  asm("         glo  ra");
  asm("         adi  8");
  asm("         plo  ra");
  asm("         ghi  ra");
  asm("         adci 0");
  asm("         phi  ra");
  asm("         ldn  ra");
  asm("         phi  rc");

  asm("         glo  ra           ; back to the first row, next byte");
  asm("         smi  23");
  asm("         plo  ra");
  asm("         ghi  ra");
  asm("         smbi 0");
  asm("         phi  ra");

  asm("         smi  0            ; the bit that marks the end");
  asm("         glo  rd");
  asm("         shlc");
  asm("         lbr  pix_row");

  asm("pix_bit: glo  rd           ; next bit of the first row");
  asm("         shl");
  asm("         lbz  pix_end      ; until the mark is shifted out");

  asm("pix_row: plo  rd");
  asm("         ldi  0            ; first row is the low two bits");
  asm("         lsnf");
  asm("         ori  03h");
  asm("         plo  re");
  asm("         ghi  rd           ; second row");
  asm("         shl");
  asm("         phi  rd");
  asm("         glo  re");
  asm("         lsnf");
  asm("         ori  0ch");
  asm("         plo  re");
  asm("         glo  rc           ; third row");
  asm("         shl");
  asm("         plo  rc");
  asm("         glo  re");
  asm("         lsnf");
  asm("         ori  30h");
  asm("         plo  re");
  asm("         ghi  rc           ; fourth row");
  asm("         shl");
  asm("         phi  rc");
  asm("         glo  re");
  asm("         lsnf");
  asm("         ori  0c0h");

  asm("         str  rf           ; store the byte twice");
  asm("         inc  rf");
  asm("         str  rf");
  asm("         inc  rf");
  asm("         lbr  pix_bit");

  asm("pix_end: irx               ; count the byte");
  asm("         ldx");
  asm("         smi  1");
  asm("         lbz  pix_ret      ; until all are done");
  asm("         stxd");
  asm("         ani  7            ; after the last byte of a row,");
  asm("         lbnz pix_col");
  asm("         glo  ra           ;  skip the three rows below it");
  asm("         adi  24");
  asm("         plo  ra");
  asm("         ghi  ra");
  asm("         adci 0");
  asm("         phi  ra");
  asm("         lbr  pix_col");
  asm("pix_ret:");
}
