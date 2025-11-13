; ASCII CHARACTERS
CR      EQU     0DH             ; CARRIAGE RETURN
LF      EQU     0AH             ; LINE FEED
CTRLZ   EQU     1AH             ; OPERATOR INTERRUPT

; CP/M BDOS FUNCTIONS
RCONF   EQU     1               ; READ CON: INTO (A)
WCONF   EQU     2               ; WRITE (A) TO CON:
RBUFF   EQU     10              ; READ A CONSOLE LINE

; CP/M ADDRESSES
RBOOT   EQU     0               ; RE-BOOT CP/M SYSTEM
BDOS    EQU     5               ; SYSTEM CALL ENTRY
TPA     EQU     100H            ; TRANSIENT PROGRAM AREA

        ORG     TPA             ; ASSEMBLE PROGRAM FOR TPA

START:  LXI     SP,STAK         ; SET UP USER'S STACK

; CONSOLE CHARACTER INTO REGISTER A MASKED TO 7 BITS
CI:     PUSH    B               ; SAVE REGISTERS
        PUSH    D
        PUSH    H
        MVI     C,RCONF         ; READ FUNCTION
        CALL    BDOS
        ANI     7FH             ; MASK TO 7 BITS
        POP     H               ; RESTORE REGISTERS
        POP     D
        POP     B
        RET

; CHARACTER IN REGISTER A OUTPUT TO CONSOLE
CO:     PUSH    B               ; SAVE REGISTERS
        PUSH    D
        PUSH    H
        MVI     C,WCONF         ; SELECT FUNCTION
        MOV     E,A             ; CHARACTER TO E
        CALL    BDOS            ; OUTPUT BY CP/M
        POP     H               ; RESTORE REGISTERS
        POP     D
        POP     B
        RET

        ; SET UP STACK SPACE
DS      64              ; 40H LOCATIONS
STAK:   DB      0               ; TOP OF STACK

; CARRIAGE RETURN, LINE FEED TO CONSOLE
TWOCR:  CALL    CCRLF           ; DOUBLE SPACE LINES
CCRLF:  MVI     A,CR
        CALL    CO
        MVI     A,LF
        JMP     CO

; MESSAGE POINTED TO BY HL OUT TO CONSOLE
COMSG:  MOV     A,M             ; GET A CHARACTER
        ORA     A               ; ZERO IS THE TERMINATOR
        RZ                      ; RETURN ON ZERO
        CALL    CO              ; ELSE OUTPUT THE CHARACTER
        INX     H               ; POINT TO THE NEXT ONE
        JMP     COMSG           ; AND CONTINUE

; MESSAGE POINTED TO BY STACK OUT TO CONSOLE
SPMSG:  XTHL                    ; GET "RETURN ADDRESS" TO HL
        XRA     A               ; CLEAR FLAGS AND ACCUMULATOR
        ADD     M               ; GET ONE MESSAGE CHARACTER
        INX     H               ; POINT TO NEXT
        XTHL                    ; RESTORE STACK FOR
        RZ                      ; RETURN IF DONE
        CALL    CO              ; ELSE DISPLAY CHARACTER
        JMP     SPMSG           ; AND DO ANOTHER

; INPUT CONSOLE MESSAGE INTO BUFFER
CIMSG:  PUSH    B               ; SAVE REGISTERS
        PUSH    D
        PUSH    H
        LXI     H,INBUF+1       ; ZERO CHARACTER COUNTER
        MVI     M,0
        DCX     H               ; SET MAXIMUM LINE LENGTH
        MVI     M,80
        XCHG                    ; INBUF POINTER TO DE REGISTERS
        MVI     C,RBUFF         ; SET UP READ BUFFER FUNCTION
        CALL    BDOS            ; INPUT A LINE
        LXI     H,INBUF+1       ; GET CHARACTER COUNTER
        MOV     E,M             ; INTO LSB OF DE REGISTER PAIR
        MVI     D,0             ; ZERO MSB
        DAD     D               ; ADD LENGTH TO START
        INX     H               ; PLUS ONE POINTS TO END
        MVI     M,0             ; INSERT TERMINATOR AT END
        POP     H               ; RESTORE ALL REGISTERS
        POP     D
        POP     B
        RET

; GET YES OR NO FROM CONSOLE
GETYN:  CALL    SPMSG
        DB      ' (Y/N)?: ',0
        CALL    CIMSG           ; GET INPUT LINE
        CALL    CCRLF           ; ECHO CARRIAGE RETURN
        LDA     INBUF+2         ; FIRST CHARACTER ONLY
        ANI     01011111B       ; CONVERT LOWER CASE TO UPPER
        CPI     'Y'             ; RETURN WITH ZERO = YES
        RZ
        CPI     'N'             ; NON-ZERO = NO
        JNZ     GETYN           ; ELSE TRY AGAIN
        CPI     0               ; RESET ZERO FLAG
        RET                     ; AND ALL DONE

INBUF:  DS      83              ; LINE INPUT BUFFER

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

; SAVE BC_0 AND DE_0 FOR LATER IN FUNCTION
        PUSH    D
        PUSH    B

; GET HIGHEST, NON-ZERO BYTE IN DIVISOR (BREAKS IF DIV BY ZERO)
        MOV     A,B
        ORA     A; MSBYTE == 0, USE LBYTE INSTEAD
        JNZ     DLSHIFT
        MOV     A,C

; C TURNS INTO countLeadingZeros(BC_0)
DLSHIFT:MVI     C,0
DLSLOOP:RAL     ; ROTATE LEFT AS MUCH AS POSSIBLE UNTIL TRUNCATE AND GET LEADING ZEROS
        JC      DNEXT
        INR     C
        JMP     DLSLOOP

; LEFT SHIFT DIVISOR AS MUCH AS POSSIBLE WITHOUT TRUNCATION
DNEXT:  MOV     A,C; A = CLZ(BC_0)
        POP     D; DE = BC_0
        CALL    LSHIFT; DE = BC_0 << (CLZ(BC_0))
        MOV     A,C
        CALL    XCHGBD; BC = DE = BC_0 << (CLZ(BC_0))

        POP     H; HL = DE_0
        LXI     D,0; DE = 0
        PUSH    D; SP->SUM = 0

        LXI     D,1; DE = 1 << CLZ(BC_0)
        CALL    LSHIFT
                ; DE = 1 << CLZ(BC_0), BC = BC_0 << (CLZ(BC_0)), HL = DE_0, SP->0
                ; BC = DIVISOR, HL = DIVIDEND, SP -> QUOTIENT, DE = BIT TO SET IN QUOTIENT

; RSHIFT DIVISOR REPEATEDLY, SUBTRACT FROM DIVIDEND AND SET BIT IN QUOTIENT WHEN DIVISOR IS >= DIVIDEND
DRSLOOP:MOV     A,B
        CMP     H
        JC      DSUB; MSBYTE OF HL > BC THEREFORE SUB
        JNZ     DIVCONT; MSYBYTE OF HL < BC (BECAUSE !(>) && !(==)) THEREFORE CONTINUE

        MOV     A,C ; MSYBYTES ARE EQUAL SO TEST LSBYTES
        CMP     L
        JC      DSUB
        JNZ     DIVCONT; LSYBYTE OF HL < BC THEREFORE CONTINUE

; HL-=BC, QUOTIENT += DE
DSUB:   CALL    NEGBC; BC *= -1
        DAD     B; HL += BC
        CALL    NEGBC; BC *= -1

        XTHL    ; SP -> SUM += DE; (DE = 1 << (CLZ(BC_0) - i)) 
        DAD     D
        XTHL

DIVCONT:CALL    XCHGBD; BC >>= 1
        MVI     A,1
        CALL    RSHIFT
        CALL    XCHGBD
        
        MVI     A,1; DE >>= 1
        CALL    RSHIFT

        MVI     A,0
        ORA     D
        ORA     E
        JNZ     DRSLOOP; for(int i = CLZ(BC_0); i >=0; i--)

        POP     D; DE = SP -> SUM
        POP     B
        POP     H
        RET


METACHAR EQU 128
PARANSTAK DS    160
SP2     EQU     $
        DW      SP2

PUSHPARAN: NOP; HL = address
        PUSH    B
        PUSH    D
        PUSH    H

        XCHG    ; DE = store value
        LXI     H,0 ; BC = SP1
        DAD     SP
        CALL    XCHGBH

        LHLD    SP2 ; HL = SP = SP2
        SPHL

        PUSH    D ; PUSH address to SP2
        DCX     H ; update SP2
        DCX     H
        SHLD    SP2

        CALL    XCHGBH ; HL = BC = SP1
        SPHL    ; SP = SP1

        POP     H ; RESTORE REGS
        POP     D 
        POP     B
        RET

POPPARAN: NOP; DE = address
        PUSH    B
        PUSH    H

        LXI     H,0 ; BC = SP1
        DAD     SP
        CALL    XCHGBH

        LHLD    SP2 ; HL = SP = SP2
        SPHL

        POP     D ; POP address from SP2
        INX     H ; update SP2
        INX     H
        SHLD    SP2

        CALL    XCHGBH ; HL = BC = SP1
        SPHL    ; SP = SP1

        POP     H ; RESTORE REGS
        POP     B
        RET

STAK:   DS      128
SP1:    DB      0
        END