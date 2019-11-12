;   CXL - Copyright (c) 1987-1989 by Mike Smedley - All Rights Reserved
;   MOVEDATA.ASM    - movedata() function for Zortech C/C++.
;
;   Requires MASM 5.1 (or compatible) to assemble.
;
;   void movedata(unsigned srcseg,unsigned srcofs,unsigned destseg,
;                 unsigned destofs,size_t numbytes);
;
;       masm /dmodl=<model> /mx cgasnow.asm ;
;

%   .MODEL  modl,C
    .DATA
    .CODE
    PUBLIC  movedata

movedata PROC USES si di ds,srcseg:WORD,srcofs:WORD,destseg:WORD,destofs:WORD,numbytes:WORD
    cld                         ; a safety precaution
    mov     ds,srcseg           ; load DS:SI with source segment:offset
    mov     si,srcofs
    mov     es,destseg          ; load ES:DI with destination segment:offset
    mov     di,destofs
    mov     cx,numbytes         ; get number of bytes to move
    shr     cx,1                ; convert to words (words=bytes/2)
    repz    movsw               ; move the words
    jnb     @1                  ; if even number of bytes, then all done
    movsb                       ; if odd number of bytes, then move 1 more byte
@1:
    ret                         ; return to caller
movedata ENDP

    END
