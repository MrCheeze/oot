.include "macro.inc"

/* assembler directives */
.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */
.set gp=64     /* allow use of 64-bit general purpose registers */

.section .data

.balign 16

/* Unused */
glabel traplink
    .word fmodf
    .word guScale
    .word guRotate
    .word guTranslate
    .word gfxprint_locate8x8
    .word gfxprint_color
    .word gfxprint_printf
    .word gfxprint_locate
    .word gfxprint_init
    .word gfxprint_open
    .word gfxprint_close
    .word gfxprint_cleanup
