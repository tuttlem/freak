
; #############################################################################

.386p
.MODEL  FLAT

; #############################################################################
; Macro 

; #############################################################################
; Data segment

_DATA SEGMENT DWORD USE32 PUBLIC 'DATA'

_DATA ENDS

; #############################################################################
; Code segment

_TEXT SEGMENT DWORD PUBLIC 'CODE'
    ASSUME CS:_TEXT, DS:_DATA

    ; #############################################################################
    ; Globals

    PUBLIC freak_getch 
    PUBLIC freak_kbhit  

    freak_getch:
        xor     ax, ax
        int     16h

        ret

    freak_kbhit:
        mov     ah, 1
        int     16h
        jz      @kbhit_no_key

        mov     al, 1
        jmp     @kbhit_done

    @kbhit_no_key: 
        xor al, al

    @kbhit_done:

        ret


_TEXT ENDS

      END