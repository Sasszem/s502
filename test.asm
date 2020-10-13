lda #5 ; 5-ot mint konstans A-ba
sta $400 ; eltaroljuk 400-ba
lda $8 ; 8-as ZPG cimet A-ba
sta $9 ; elmentjuk 9-be (zpg)
; begin a loop
; init
ldx #0
lda #$23
.define SCREEN $400
.define ASD #4
.define B #$10
.define C @B
;.include "c64.asm"
.ifdef A
.print Defining ASD
.define ASD
.endif
; loop body
loop:
    sta %SCREEN, X
    inx
    cpx 10
    bne loop ; loop until x is 10
nem
opk
odF
kib
asz
ott
hos
szu
sor
ami
kia
kas
ztj
aaf
ord
ito
tis