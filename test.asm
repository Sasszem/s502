lda #5 ; 5-ot mint konstans A-ba
ldx #0;
.define SCREEN $400
; loop body
loop:
    sta %SCREEN, X
    inx
    cpx #10
    bne loop ; loop until x is 10

rts
