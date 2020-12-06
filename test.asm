; Implements a super-simple program for the C=64
; (changes border color in in infinite loop with some busy wait loops)
; creates a PRG file structure (see link bellow), so the result can be just dropped into an emulator (for ex. VICE)
; or written to a 5/4' floppy if you want

; PRG file:             https://michaelcmartin.github.io/Ophis/book/x72.html
; C=64 BASIC tokens:    https://www.c64-wiki.com/wiki/BASIC_token

.include c64.asm


; #############################
; # PRG FILE AND BASIC LOADER #
; #############################

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

; ##################
; # ACTUAL PROGRAM #
; ##################

; clear screen
; from http://www.rarityguide.com/forums/commodore-basic-classroom/218-how-do-i-clear-screen-commodore-64-a.html
lda #147
jsr &C64_CHROUT

; print text
ldx #>&BEGINTEXT
ldy #<&BEGINTEXT
jsr &print

; ##################
; # INITIAL SCREEN #
; ##################

; wait screen with fancy border effect 
loop:
.ifdef CRAZY_EFFECT
    lda &C64_BORDER
    inx
    adc #0 ; if X overflow we add 1 from carry
    sta &C64_BORDER
.endif
    ; check if spacebar is pressed
    lda &C64_KEY
    cmp #60
    bne &loop

; gray initial border
lda #11
sta &C64_BORDER
; black background
lda #0
sta &C64_BACKGROUND
; white text
lda #5
jsr &C64_CHROUT

; clear it again
lda #147
jsr &C64_CHROUT

; ####################
; # ACTUAL BALL DEMO #
; ####################
.ifndef WAIT
    .define WAIT 3000
.endif
gameloop:
    ; we use a 16 bit counter
    ; and only run logic if it's at a spefific value
    ; but still check exit condition regardless of its value 

; 16 bit increment
    inc &WAITCOUNTER
    bne &gameloop_wait_compare
    inc &WAITCOUNTER_H

gameloop_wait_compare:
; 16 bit comare
    lda #<@WAIT
    cmp &WAITCOUNTER
    bne &gameloop_keycheck
    lda #>@WAIT
    cmp &WAITCOUNTER_H
    bne &gameloop_keycheck

; following is "game" logic

; reset counter
    lda #0
    sta &WAITCOUNTER
    sta &WAITCOUNTER_H

gameloop_undraw:
    ; un-draw the ball
    jsr &make_balladdr
    lda #$20
    ldx #0
    sta (&C64_ZPGM1, X)

; update X position and flip velocity if we reached a border
gameloop_x:
    ; add VX to X
    lda &BALLX
    clc
    adc &BALLVX
    sta &BALLX
    ; if at 0
    beq &gameloop_flipX
    ; if at last column
    cmp #39
    beq &gameloop_flipX
    jmp &gameloop_y ; skip flipping

gameloop_flipX:
    lda #0
    sec
    sbc &BALLVX
    sta &BALLVX
.ifdef BORDERCHANGE
    inc &C64_BORDER
.endif

; update Y position and velocity
; kinda the same deal as with X 
gameloop_y:
    ; add VX to X
    lda &BALLY
    clc
    adc &BALLVY
    sta &BALLY
    ; if at 0
    beq &gameloop_flipY
    ; if at last row
    cmp #24
    beq &gameloop_flipY
    jmp &gameloop_draw ; skip flipping

gameloop_flipY:
    lda #0
    sec
    sbc &BALLVY
    sta &BALLVY
.ifdef BORDERCHANGE
    inc &C64_BORDER
.endif
gameloop_draw:
    ; draw the ball
    jsr &make_balladdr
    lda #87
    ldx #0
    sta (&C64_ZPGM1, X)

gameloop_keycheck:
    ; check if Q is pressed
    lda 197
    cmp #62
    bne &gameloop

; ####################
; # CLEANUP AND EXIT #
; ####################

; undo everything to the screen
jsr &C64_SCINIT

; print text
ldx #>&BYETEXT
ldy #<&BYETEXT
jsr &print

rts

; #############
; # VARIABLES #
; #############

; wait counter to slow down the main loop of the program 
WAITCOUNTER:
.data 0
WAITCOUNTER_H:
.data 0
; damn I miss offsetting...

; ball position and velocity
BALLX:
.data 5
BALLY:
.data 3
BALLVX:
.data 1
BALLVY:
.data 1

; text to print at the beginning
BEGINTEXT:
.data "S502" 32 "ASSEMBLER" 32 "SANITY" 32 "TEST!" $d $d
.data "PREESS" 32 "SPACE" 32 "TO" 32 "START!" $d
.data "(PRESS" 32 "Q" 32 "TO" 32 "QUIT" 32  "WHEN" 32 "STARTED)" $D 0

; text to print at the end
BYETEXT:
.data 147 "GOODBYE!" 0

; # ##########
; # ROUTINES #
; ############

; print a string using CHROUT
; address should be in y:x
; based on https://www.c64-wiki.com/wiki/Assembler_Example
print:
    sty *&C64_ZPGM1
    stx *&C64_ZPGM2
    ldy #$0
print_loop:
    lda ($fb), y
    beq &print_quit
    jsr &C64_CHROUT
    iny
    jmp &print_loop
print_quit:
    RTS

; set up address of ball (in screen memory) in zeropage buffer
; has no parameters
; calculates SCREEN + 40*Y + X 
make_balladdr:
    ; set it to SCREEN
    lda #<&C64_SCREEN
    sta *&C64_ZPGM1
    lda #>&C64_SCREEN
    sta *&C64_ZPGM2
    ldx &BALLY
make_balladdr_addloop:
    cpx #0
    beq &make_balladdr_addx
    dex
    lda *&C64_ZPGM1
    clc
    adc #40
    sta *&C64_ZPGM1
    bcc &make_balladdr_addloop
    inc *&C64_ZPGM2
    jmp &make_balladdr_addloop

make_balladdr_addx: 
    lda *&C64_ZPGM1
    clc
    adc &BALLX
    sta *&C64_ZPGM1
    bcc &make_balladdr_exit
    inc *&C64_ZPGM2

make_balladdr_exit:
    rts

set_ball:
