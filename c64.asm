; just a few C64 memory locations 
;
; based on:
;
; C=64 memory map:      http://sta.c64.org/cbm64mem.html
; C=64 KERNAL rom:      https://sta.c64.org/cbm64krnfunc.html


; zeropage 4-byte buffer
; quite useful for indirect addressing
; and also safe to use
.org $fb
C64_ZPGM1:
.org $fc
C64_ZPGM2:
.org $fd
C64_ZPGM3:
.org $fe
C64_ZPGM4:

; default screen memory (1000 bytes for 40*25)
.org $400
C64_SCREEN:

; currently pressed key
.org 197
C64_KEY:

; kernal CHROUT routine
.org $FFD2
C64_CHROUT:
; kernal VIC init routine
.org $ff81	
C64_SCINIT:

; VIC II border color
.org $D020
C64_BORDER:

; VIC II background color
.org $D021
C64_BACKGROUND:

; VIC II raster line register - can be used to set interrupt
.org $D012
C64_RASTLINE:

; VIC II interrupr registers
.org $D019
C64_INTST:
.org $D01A
C64_INTCTRL:

