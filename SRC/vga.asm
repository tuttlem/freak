
; #############################################################################

.386p
.MODEL  FLAT

; #############################################################################
; Macro 

outp MACRO port, value
    mov dx, port
    mov al, value
    out dx, al
ENDM

; #############################################################################
; Data segment

_DATA SEGMENT DWORD USE32 PUBLIC 'DATA'

    freak_vga       DD          0A0000h

_DATA ENDS

; #############################################################################
; Code segment

_TEXT SEGMENT DWORD PUBLIC 'CODE'
    ASSUME CS:_TEXT, DS:_DATA

    ; #############################################################################
    ; Globals

    PUBLIC freak_vga

    PUBLIC freak_set_modex 
    PUBLIC freak_set_text  
    PUBLIC freak_wait_vsync

    freak_set_modex:
        mov     ax, 0013h
        int     10h
    
        outp 03d4h, 11h

        mov dx, 03d5h
        in  al, dx
        and al, 7fh
        mov bl, al

        outp 03d4h, 11h
        outp 03d5h, bl

        outp 03c2h, 0e3h
        outp 03d4h, 000h
        outp 03d5h, 05fh
        outp 03d4h, 001h
        outp 03d5h, 04fh
        outp 03d4h, 002h
        outp 03d5h, 050h
        outp 03d4h, 003h
        outp 03d5h, 082h
        outp 03d4h, 004h
        outp 03d5h, 054h
        outp 03d4h, 005h
        outp 03d5h, 080h
        outp 03d4h, 006h
        outp 03d5h, 00dh
        outp 03d4h, 007h
        outp 03d5h, 03eh
        outp 03d4h, 008h
        outp 03d5h, 000h
        outp 03d4h, 009h
        outp 03d5h, 041h
        outp 03d4h, 010h
        outp 03d5h, 0eah
        outp 03d4h, 011h
        outp 03d5h, 0ach
        outp 03d4h, 012h
        outp 03d5h, 0dfh
        outp 03d4h, 013h
        outp 03d5h, 028h
        outp 03d4h, 014h
        outp 03d5h, 000h
        outp 03d4h, 015h
        outp 03d5h, 0e7h
        outp 03d4h, 016h
        outp 03d5h, 006h
        outp 03d4h, 017h
        outp 03d5h, 0e3h
        outp 03c4h, 001h
        outp 03c5h, 001h
        outp 03c4h, 004h
        outp 03c5h, 006h
        outp 03ceh, 005h
        outp 03cfh, 040h
        outp 03ceh, 006h
        outp 03cfh, 005h

        mov dx, 03dah
        in  al, dx
        
        outp 03c0h, 010h or 020h
        outp 03c0h, 041h

        mov dx, 03dah
        in  al, dx
        
        outp 03c0h, 013h or 020h
        outp 03c0h, 0

        outp 03d4h, 011h

        mov dx, 03d5h
        in  al, dx
        or  al, 80h
        mov bl, al

        outp 03d4h, 11h
        outp 03d5h, bl

        ret

    freak_set_text:
        mov     ax, 0003h
        int     10h
        ret

    freak_wait_vsync:
        mov     dx, 03dah
    @vsync1:
        in      al, dx
        test    al, 08h
        jz      @vsync1
    @vsync2:
        in      al, dx
        test    al, 08h
        jnz     @vsync2

        ret

_TEXT ENDS

      END