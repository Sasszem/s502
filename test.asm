; Implements a super-simple program for the C=64
; (changes border color in in infinite loop with some busy wait loops)
; creates a PRG file structure (see link bellow), so the result can be just dropped into an emulator (for ex. VICE)
; or written to a 5/4' floppy if you want

; PRG file:             https://michaelcmartin.github.io/Ophis/book/x72.html
; C=64 memory map:      http://sta.c64.org/cbm64mem.html
; C=64 KERNAL rom:      https://sta.c64.org/cbm64krnfunc.html
; C=64 clear screen:    http://www.rarityguide.com/forums/commodore-basic-classroom/218-how-do-i-clear-screen-commodore-64-a.html
; C=64 BASIC tokens:    https://www.c64-wiki.com/wiki/BASIC_token

.include c64.asm

.data w:$0801 ; PRG file header

.org $0801 ; we work in BASIC program memory

; 1st (and only) BASIC line
.data w:&lastline w:10 $9e "2064" 0
; next line PTR, line number, BASIC token (sys), rest of line and trailing 0

lastline:
.data w:0
; NULL as next line ptr indicates end of program

.pad $810 0
; we want to start at 2064


begin:
; clear screen
lda #147
jsr &CHROUT

; goal of today: make this compile and work


; Funny effect

.define WAIT 32
loop:
    inx
    cpx #@WAIT
    bne &loop

    inc &BORDER
    jmp &loop