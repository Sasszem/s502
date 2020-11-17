.ifndef CNT
    .print NOT DEFINED, INCLUDING
    .define CNT 5
    .include recurse.asm
.endif

.printc CNT

.ifdef CNT
    .print DEFINED
.endif
