        LXI     SP,SP1
XCHGBD: PUSH    D; SWAP(BC, DE)
        PUSH    B 
        POP     D 
        POP     B
        RET
XCHGBH: PUSH    H; SWAP(BC, HL)
        PUSH    B 
        POP     H
        POP     B
        RET

MULTBYT:PUSH    D; BC = A * DE
        PUSH    H
        LXI     H,0
        MVI     C,8; INIT COUNTER
MLOOP:  RAR     ; TEST LSB OF MULTIPLIER
        JNC     NEXT
        DAD     D; ADD MULTIPLICAND TO PP IF ONE
NEXT:   XCHG    ; LSHIFT MULTIPLICAND
        DAD     H
        XCHG
        DCR     C; REPEAT FOR LOOP WHILE COUNTER IS > 0
        JNZ     LOOP
        RAR

        PUSH    H
        POP     B
        POP     H
        POP     D
        RET

MULT2B: PUSH    B; DE *= BC
        PUSH    H
        MOV     H,B; KEEP TRACK OF HIGH BYTE OF MULTIPLIER

        MOV     A,C; MULT LOW BYTE OF MULTIPLIER WITH MULTIPLICAND
        CALL    MULTBYT

        MOV     A,H; MOVE HIGH BYTE OF MULTIPLIER TO A
        PUSH    B; MOVE FIRST PRODUCT TO HL
        POP     H
        CALL    MULTBYT; MSB TIMES MULTIPLICAND

        MOV     B,C; PRODUCT *= 256
        MVI     C,0; SUM += PRODUCT
        DAD     B

        XCHG
        POP     H
        POP     B
        RET

NEGBC:  MOV     A,C; BC = -BC
        CMA     ; C = !C
        MOV     C,A

        MOV     A,B 
        CMA     ; B = !B
        MOV     B,A

        INX B   ; BC++

        RET     ; BC = !BC + 1 = -BC

BINSUB: PUSH H; DE -= BC
        CALL    NEGBC
        XCHG
        DAD B
        XCHG
        POP H
        RET

LSHIFT: INR     A; DE <<= A
        XCHG
LLOOP:  DCR     A; for(int i = A+1; --i > 0;)
        JZ      LEND
        DAD     H
        JMP     LLOOP
LEND:   XCHG
        RET

RSHIFT: PUSH    B; DE >>= A
        MOV     C,A; C = A
RLOOP:  STC     ; CLEAR CARRY BIT
        CMC
        
        MOV     A,D; RSHIFT MSBYTE
        RAR
        MOV     D,A

        MOV     A,E; RSHIFT LSBYTE WITH CARRY IN FROM MSBYTE
        RAR
        MOV     E,A

        DCR     C; for(int c = a; c > 0; c--)
        JNZ     RLOOP
        POP     B 
        RET

DIV:    PUSH    H; DE /= BC
        PUSH    B

        PUSH    D
        PUSH    B

        MOV     A,B
        ORA     A; MSBYTE = 0, USE LBYTE INSTEAD
        JNZ     DLSHIFT
        MOV     A,C

DLSHIFT:MVI     C,0
DLSLOOP:RAL     ; ROTATE LEFT AS MUCH AS POSSIBLE AND GET LEADING ZEROS
        JC      DNEXT
        INR     C
        JMP     DLSLOOP

DNEXT:  MOV     A,C; A = CLZ(BC_0)
        POP     D; DE = BC_0
        CALL    LSHIFT; DE = BC_0 << (CLZ(BC_0))
        MOV     A,C

        PUSH    D 
        POP     B; BC = DE
        POP     H; HL = DE_0

        LXI     D,0
        PUSH    D; SP->0 = SUM

        LXI     D,1; DE = 1 << CLZ(BC_0)
        CALL    LSHIFT
                ; DE = 1 << CLZ(BC_0), BC = BC_0 << (CLZ(BC_0)), HL = DE_0, SP->0
DRSLOOP:MOV     A,B
        CMP     H
        JC      DSUB; MSBYTE OF HL > BC THEREFORE SUB
        JNZ     DIVCONT; MSYBYTE OF HL < BC THEREFORE CONTINUE

        MOV     A,C ; MSYBYTES ARE EQUAL SO TEST LSBYTES
        CMP     L
        JC      DSUB
        JNZ     DIVCONT; LSYBYTE OF HL < BC THEREFORE CONTINUE

DSUB:   CALL NEGBC; HL-=BC
        DAD     B
        CALL NEGBC

        XTHL    ; SP -> SUM += DE; (DE = 1 << (CLZ(BC_0) - i)) 
        DAD     D
        XTHL

DIVCONT:CALL XCHGBD
        MVI     A,1; BC >>= 1
        CALL RSHIFT
        CALL XCHGBD
        
        MVI     A,1; DE >>= 1
        CALL RSHIFT

        MVI     A,0
        ORA     D
        ORA     E
        JNZ     DRSLOOP; for(int i = CLZ(BC_0); i >=0; i--)

        POP     D; DE = SP -> SUM
        POP     B
        POP     H
        RET


METAC:  EQU     128
STR:    DS      83
MIDNUM: DS      160
SP2:    EQU     $
        DW      SP2

; DE = intermediate value, HL = str replacement pointer, A = str length
SETMID: PUSH    H 
        PUSH    D 
        PUSH    B

        CALL    XCHGBD

        LXI     H,0
        DAD     SP
        XCHG

        LHLD    SP2
; BC = VALUE, DE = SP, HL = SP2
        PUSH    B; PUSH VALUE TO INTERMEDIATE STACK

        DCX     H; UPDATE SP2
        DCX     H
        SHLD    SP2

        XCHG    ; RESET SP
        SPHL

        POP     B
        POP     D
        POP     H

        PUSH    H

        LHLD    SP2 ; HL = INDEX OF INTERMEDIATE
        LXI     B,SP2
        CALL    NEGBC
        DAD     B

        POP     H
        PUSH    H

        
RSMIDS: 


STAK:   DS      128
SP1:    DB      0
        END