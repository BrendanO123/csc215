; ASCII CHARACTERS
CR      EQU     0DH             ; CARRIAGE RETURN
LF      EQU     0AH             ; LINE FEED
CTRLZ   EQU     1AH             ; OPERATOR INTERRUPT
ZERO    EQU     30H

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

;---------------------------------------------------

START1:
        CALL CIEQUATION
        CALL SPMSG
        DB 'THATS ALL, FOLKS!',0
        JMP RBOOT   ; END TEST PROGRAM

;---------------------------------------------------

; INPUT CONSOLE EQUATION INTO BUFFER
CIEQUATION:
        CALL SPMSG
        DB 'HELLO THERE! PLEASE INPUT AN EQUATION.',CR,LF,0

        CALL CIMSG
        LXI H,INBUF+2   ; POINT TO FIRST CHARACTER
        CALL ENDL
EVALEQUATION:
        MOV A,M
        CPI 0
        JZ EQFINALEVAL
        
        CPI '('
        JZ EQLGROUPING

        CPI ')'
        JZ EQPARENEVAL

        INX H
        JMP EVALEQUATION
EQFINALEVAL: ; EVAL FINAL GROUPING
        DCX H
        XCHG ; SAVE HL
        LXI H,INBUF+2   ; POINT TO FIRST CHARACTER
        CALL AUXPUSH ; push as start address
        XCHG ; restore HL
        CALL EVAL ; eval entire string
        CALL PRINTDE ; print result
        CALL ENDL
        RET


EQLGROUPING:
        CALL AUXPUSH ; push address to aux stack
        INX H ; update search address
        JMP EVALEQUATION ; return

EQPARENEVAL:
        PUSH H
        CALL EVAL 
        POP H
        INX H
        JMP EVALEQUATION

;---------------------------------------------------

EVAL: ; HL = end address of string (address of ')' in string)
        PUSH H ; this is little patch bc we overwrote HL before pushing it as ending address
        ; to fix this, I am just pushing this here, retrieving it below, pushing it to aux stack, and continuing
        
        CALL AUXPOP ; DE = start address  of string (address of '(' in string)

        PUSH D ; BC = start address, HL = search address = start address (for now)
        PUSH D
        POP B
        POP H

        CALL XCHGBH ; save start address for next loop
        CALL AUXPUSH 
        CALL XCHGBH

        XTHL ; get original end address
        CALL AUXPUSH ; save end address for end check
        POP H ; restore HL to search index

        PUSH B ; save start address for repeated use

EVALLOOP:
        CALL AUXPOP ; load in end address from aux stack
        XCHG ; and push back onto aux stack 
        CALL AUXPUSH ; (HL is pushed but AUXPOP puts end address into DE
        XCHG ; therefore DE is put into HL before AUXPUSH then restored)

        PUSH H ; save HL
        CALL NEGATEDE ; compare with HL
        DAD D 
        MOV A,H 
        RAL ; sets carry flag if end address in DE > HL (sub resulted in a negative result)

        POP H ; restore HL

        JNC EVALEOS ; DO SOMETHING ON END

; continue eval if HL < DE
        MOV A,M
        PUSH H
        LHLD CURRENTPASS
        JMP SWITCH

EVALEOS:
        LHLD CURRENTPASS ; load in current pass 
        LXI D,ADDPASS ; and addpass 

        CALL NEGATEDE ; check if currentpass is addpass
        DAD D 
        MOV A,H 
        ORA L ; sets zero flag if end address in DE == HL

        JZ EVALEND ; if already in addpass, go to end. Else:

        LXI H,ADDPASS ; set switch logic to add pass logic
        SHLD CURRENTPASS

        CALL AUXPOP ; save end address in HL
        XCHG

        POP B ; clear starting point 
        CALL AUXPOP ; load initial starting point
        CALL XCHGBD
        PUSH B ; store reloaded starting point

        CALL XCHGBH ; push start address
        CALL AUXPUSH 
        CALL XCHGBH ; push end address
        CALL AUXPUSH

        PUSH B ; set search index
        POP H

        JMP EVALLOOP; run add-sub loop
EVALEND:
        LXI H,MULTPASS 
        SHLD CURRENTPASS

        POP B ; clear starting point

        CALL AUXPOP ; load in end address (end parenthesis included)
        XCHG

        CALL AUXPOP ; load in starting address
        CALL XCHGBD

        PUSH B
        CALL XCHGBH ; load final result into DE
        MOV A,M 
        CPI '('
        JNZ EvalEndParse
        INX H
EvalEndParse:
        CALL PARSENUM
        CALL XCHGBH
        POP B

        CALL SETINTER ; store and overwrite "()"
        RET

;---------------------------------------------------

        EVALPARSE:
                CALL XCHGBH ; DE = number from BC to operator/HL
                CALL PARSENUM
                CALL XCHGBH

                CALL XCHGBD ; save to BC bc BC = HL therefore useless for now

                INX H
                CALL PARSENUM ; DE = number from HL/operator on to next operator

                CALL XCHGBD ; fix order of numbers
                RET

        EVALLOOPCONT:
                POP B ; get start address of intermediate
                PUSH B ; restore stack
                
                DCX H ; decrement HL to not overflow bounds
                CALL SETINTER ; set
                INX H ; increment HL back

                JMP EVALLOOP ; return to loop

CURRENTPASS: DW MULTPASS
                MULTPASS: DW '*',EVALMULT,'<',EVALLS,'>',EVALRS,'/',EVALDIV,'%',EVALMOD,'+',EVALSKIP,'-',EVALSKIP,'(',EVALSKIP,')',EVALSKIP,DEFAULT,EVALDEFAULT
                ADDPASS: DW '*',EVALSKIP,'<',EVALSKIP,'>',EVALSKIP,'/',EVALSKIP,'%',EVALSKIP,'+',EVALADD,'-',EVALSUB,'(',EVALSKIP,')',EVALSKIP,DEFAULT,EVALDEFAULT

                EVALSKIP: ; Case ('(' | ')' or whatever operator is ignored):
                        POP H ; move up HL to pass operator
                        INX H

                        POP B ; clear previous value of B

                        PUSH H ; move up BC to next parameter
                        POP B

                        PUSH B ; save new value of B

                        JMP EVALLOOP

                EVALDEFAULT: ; Default:
                        POP H
                        MOV A,M ; test if is skip char to avoid misclassifying intermediates as operators
                        ANI INTERBIT128
                        JZ EVALDEFAULTNEXT ; if it is a normal num, just walk through it
                        CALL PARSEINTER ; if it is an intermediate, use parse_intermediate to skip HL past intermediate
                        JMP EVALLOOP ; return to loop
                EVALDEFAULTNEXT:
                        INX H ; skip char
                        JMP EVALLOOP ; return to loop
        
                EVALMULT: ; Case *:
                        POP H
                        CALL EVALPARSE
                        CALL MULT16
                        JMP EVALLOOPCONT
                
                EVALLS: ; Case <:
                        POP H
                        CALL EVALPARSE
                        MOV A,C
                        CALL LSHIFT
                        JMP EVALLOOPCONT

                EVALRS: ; Case >:
                        POP H
                        CALL EVALPARSE
                        MOV A,C
                        CALL RSHIFT
                        JMP EVALLOOPCONT

                EVALDIV: ; Case /:
                        POP H
                        CALL EVALPARSE
                        CALL INTDIV
                        JMP EVALLOOPCONT
                
                EVALMOD: ; Case %:
                        POP H
                        CALL EVALPARSE
                        CALL RECTMOD
                        JMP EVALLOOPCONT

                EVALADD:  ; Case +:
                        POP H
                        CALL EVALPARSE
                        XCHG 
                        DAD B
                        XCHG
                        JMP EVALLOOPCONT

                EVALSUB:  ; Case -:
                        POP H
                        CALL EVALPARSE
                        CALL SUB16
                        JMP EVALLOOPCONT

;---------------------------------------------------

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

MULT8:
        PUSH    D; BC = A * DE
        PUSH    H
        LXI     H,0 ; HL = PP
        MVI     C,8; INIT COUNTER
MLOOP:  RAR     ; TEST LSB OF MULTIPLIER
        JNC     MNEXT
        DAD     D; ADD MULTIPLICAND TO PP IF ONE
MNEXT:  XCHG    ; LSHIFT MULTIPLICAND
        DAD     H
        XCHG
        DCR     C; REPEAT FOR LOOP WHILE COUNTER IS > 0
        JNZ     MLOOP

        CALL XCHGBH
        POP     H
        POP     D
        RET

MULT16: 
        PUSH    B; DE *= BC
        PUSH    H
        MOV     H,B; KEEP TRACK OF HIGH BYTE OF MULTIPLIER

        MOV     A,C; MULT LOW BYTE OF MULTIPLIER WITH MULTIPLICAND
        CALL    MULT8

        MOV     A,H; MOVE HIGH BYTE OF MULTIPLIER TO A
        PUSH    B; MOVE FIRST PRODUCT TO HL
        POP     H
        CALL    MULT8; MSB TIMES MULTIPLICAND

        MOV     B,C; PRODUCT *= 256
        MVI     C,0; SUM += PRODUCT
        DAD     B

        XCHG
        POP     H
        POP     B
        RET

NEGATEDE:
        CALL XCHGBD
        CALL NEGATEBC
        CALL XCHGBD
        RET
NEGATEBC:  
        MOV     A,C; BC = -BC
        CMA     ; C = !C
        MOV     C,A

        MOV     A,B 
        CMA     ; B = !B
        MOV     B,A

        INX B   ; BC++

        RET     ; BC = !BC + 1 = -BC

SUB16:  
        PUSH H; DE -= BC
        PUSH B
        CALL    NEGATEBC
        XCHG
        DAD B
        XCHG
        POP B
        POP H
        RET

LSHIFT: 
        INR     A; DE <<= A
        XCHG
LSLOOP: DCR     A; for(int i = A+1; --i > 0;)
        JZ      LEND
        DAD     H
        JMP     LSLOOP
LEND:   XCHG
        RET

RSHIFT: 
        PUSH    B; DE >>= A
        MOV     C,A; C = A
RSLOOP: STC     ; CLEAR CARRY BIT
        CMC
        
        MOV     A,D; RSHIFT MS BYTE
        RAR
        MOV     D,A

        MOV     A,E; RSHIFT LS BYTE WITH CARRY IN FROM MS BYTE
        RAR
        MOV     E,A

        DCR     C; for(int c = a; c > 0; c--)
        JNZ     RSLOOP
        POP     B 
        RET

CLZ: ; A = CLZ(BC) (BREAKS IF ZERO)
        PUSH B
; GET HIGHEST, NON-ZERO BYTE IN DIVISOR (BREAKS IF DIV BY ZERO)
        MOV     A,B
        ORA     A; MS BYTE == 0, USE LS BYTE INSTEAD
        JNZ     CLZLSHIFT
        MOV     A,C
        MVI     C,8
        JMP     CLZLSHIFTLOOP

; C TURNS INTO countLeadingZeros(BC)
CLZLSHIFT:
        MVI     C,0
CLZLSHIFTLOOP:
        RAL     ; ROTATE LEFT AS MUCH AS POSSIBLE UNTIL TRUNCATE AND GET LEADING ZEROS
        JC      CLZRET
        INR     C
        JMP     CLZLSHIFTLOOP
CLZRET: MOV     A,C
        POP B
        RET

RECTMOD:
        PUSH B
        CALL RECTSIGN
        CALL MOD0
        POP B
        LDA FLAG
        RAR ; SAMPLE LSB flag bit
        RNC
        CALL NEGATEDE
        MVI A,0; CLEAR FLAGS
        STA FLAG
        RET
INTDIV:
        PUSH B
        CALL RECTSIGN
        CALL INTDIV0
        POP B
        LDA FLAG
        RAR ; SAMPLE LSB flag bit
        RNC
        CALL NEGATEDE
        MVI A,0; CLEAR FLAGS
        STA FLAG
        RET
RECTSIGN:
        MVI A,0; CLEAR FLAGS
        STA FLAG

        MOV A,D ; TEST DE SIGN BIT
        RAL
        JNC RECTSIGNNEXT
        LDA FLAG
        XRI 01H
        STA FLAG
        CALL NEGATEDE ; FORCE POSITIVE AND UPDATE FLAG IF NEGATIVE
RECTSIGNNEXT:
        MOV A,B ; TEST DE SIGN BIT
        RAL
        RNC
        LDA FLAG
        XRI 01H
        STA FLAG
        CALL NEGATEBC
        RET
MOD0:
        PUSH H ; DE %= BC
        CALL DIVMOD
        XCHG
        POP H
        RET
INTDIV0:
        PUSH H ; DE //= BC
        CALL DIVMOD
        POP H
        RET

DIVMOD:
        PUSH    B; DE = DE // BC, HL = DE % BC

; SAVE BC_0 AND DE_0 FOR LATER IN FUNCTION
        PUSH    D
        PUSH    B

; get countLeadingZeros(BC_0)
        CALL CLZ ; A = CLZ(BC_0)
        MOV     C,A
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
        JC      DIVSUB; MS BYTE OF HL > BC THEREFORE SUB
        JNZ     DIVCONT; MS BYTE OF HL < BC (BECAUSE !(>) && !(==)) THEREFORE CONTINUE

        MOV     A,C ; MS BYTES ARE EQUAL SO TEST LS BYTES
        CMP     L
        JC      DIVSUB
        JNZ     DIVCONT; LS BYTE OF HL < BC THEREFORE CONTINUE

; HL-=BC, QUOTIENT += DE
DIVSUB:   CALL    NEGATEBC; BC *= -1
        DAD     B; HL += BC
        CALL    NEGATEBC; BC *= -1

        XTHL    ; SP -> SUM += DE; (DE = 1 << (CLZ(BC_0) - i)) 
        DAD     D
        XTHL

DIVCONT:
        CALL    XCHGBD; BC >>= 1
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
        RET

;---------------------------------------------------

DEFAULT EQU 256 ; if the second byte in the case is non-zero, then we know a single byte CASE was not entered
    ; therefore we assume this is the default case. To get this in a DW we jut load in 256 as a 16 bit number.

SWITCH: ; A = switch character, HL = pointer to DW with case,hook_address,case,hook_address,(#>255),default_case_hook_address
        ; HL is saved
        ; stack has return address on top then any previous pushes below
        ; so if you run PUSH H; LXI H,DW_THING; CALL SWITCH, then you will need to run POP H; XTHL
        ; to get the return address to the top of the stack and HL back from the push

        PUSH H
SWITCHLOOP:
        PUSH PSW ; save A
        INX H
        MOV A,M ; test second byte of CASE value
        DCX H
        CPI 0 ; if second byte value is not 0, the case was not a single byte number or ASCII character
        JNZ SWITCHDEFAULT ; use case as default and execute case

        POP PSW ; restore A
        CMP M ; compare with CASE value
        JZ SWITCHNEXT ; if is equal, execute case
        INX H ; else HL += 4 and continue
        INX H
        INX H
        INX H
        JMP SWITCHLOOP

SWITCHDEFAULT:
        POP PSW
SWITCHNEXT:
        PUSH D ; load hook into HL
        INX H ; load LS byte
        INX H
        MOV E,M
        INX H ; load MS byte
        MOV D,M
        XCHG
        POP D

        XTHL ; put jump address on top of stack for return call and restore HL_0
        RET ; jump to hook

;---------------------------------------------------

INTERMASK EQU 127
INTERBIT128 EQU 128

PARSENUM: ; HL = first character of parameter/number
        MOV A,M ; TEST IF FIRST CHAR IS '-'
        CPI '-'
        JNZ PARSENUMNEXT

; (NEG)
        INX H ; GET FIRST REAL CHAR
        MOV A,M
        DCX H

PARSENUMNEXT:
        ANI INTERBIT128
        JNZ PARSEINTER
        JMP PARSEINT

PARSEINTER: ; HL = address of first byte of intermediate, stores into DE
        PUSH PSW
        LXI D,0
        MVI A,0
        STA FLAG


        MOV A,M
        CPI '-'
        JNZ PARSEINTERNEXT
        MVI A,1 ; if negative, set flag and skip '-'
        STA FLAG
        INX H


PARSEINTERNEXT:
        MOV A,M ; GET SKIP BYTE
        ANI INTERMASK ; MASK OUT INTERMEDIATE FLAG
        MOV E,A
        DAD D ; SKIP THAT MANY BYTES

        INX H; POINT TO FIRST BYTE OF INTERMEDIATE
        MOV E,M ; LOAD DE
        INX H
        MOV D,M

        LDA FLAG ; test LSB of flag to see if negative
        RAR

        JNC PARSEINTERRET ; if negative, negate and clear flag
        CALL NEGATEDE 
        MVI A,0
        STA FLAG

PARSEINTERRET:
        POP PSW ; RESTORE AND RETURN
        INX H
        RET

SETINTER: ; HL = right-most address of sequence, DE = value, BC = left-most address

        PUSH H
        PUSH B

        CALL NEGATEBC ; CONSTRUCT SKIP BYTE
        DAD B ; GET SKIP DIST
        MOV A,L
        SUI 2 ; TO ACCOUNT FOR BYTES RESERVED TO STORE VALUE
        ORI INTERBIT128 ; SET MSB TO DENOTE AS NON-ASCII

        POP H ; RESTORE BC_0
        MOV M,A ; SET SKIP BYTE

        XTHL ; RESTORE HL_0 AND BC_0
        POP B

        MOV M,D ; SET VALUE
        DCX H
        MOV M,E

        DCX H ; MAKE SURE NEXT BIT IS MARKED AS INTERMEDIATE
        MOV A,M
        ORI INTERBIT128
        MOV M,A
        
        INX H ; RESTORE AND RETURN
        INX H
        RET

;---------------------------------------------------

        DS    160
SP2     EQU     $
        DW      SP2

AUXPUSH: ; HL = address
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

        POP     H ; RESTORE REGISTERS
        POP     D 
        POP     B
        RET

AUXPOP: ; DE = address
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

        POP     H ; RESTORE REGISTERS
        POP     B
        RET

AUXPUSH8: ; A = address
        PUSH H
        LHLD SP2
        DCX H
        MOV M,A
        SHLD SP2
        POP H
        RET

AUXPOP8: ; A = address
        PUSH H
        LHLD SP2
        MOV A,M
        INX H
        SHLD SP2
        POP H
        RET

        DS      128
STAK:   DB      0

;---------------------------------------------------

PRINTDE: ; PRINTS A SIGNED 16-BIT INTEGER IN DE
        ; Save registers
        PUSH PSW
        PUSH B
        PUSH D
        PUSH H

        LHLD SP2 ; Save AUX SP
        PUSH H

        MVI A,0 ; Zero Terminate
        CALL AUXPUSH8
        LXI B,10

        MOV A,D ; Rectify sign
        RAL
        JNC PRINTINTLOOP
        MVI A,'-'
        CALL CO
        CALL NEGATEDE
PRINTINTLOOP:
        CALL DIVMOD ; get digit and divide
        MVI A,'0'
        ADD L
        CALL AUXPUSH8 ; push digit to end of message
        MOV A,D
        ORA E
        JNZ PRINTINTLOOP ; loop

        LHLD SP2 ; print message
        CALL COMSG

        POP H ; restore AUX SP
        SHLD SP2

        POP H ; restore registers
        POP D
        POP B
        POP PSW
        RET

; INPUT (CHAR POINTER) GOES IN HL, RESULT GOES IN DE
PARSEINT:
        PUSH B
        LXI D,00H ; CLEAR DE REGISTERS IN PREPARATION
        LXI B,10
        MOV A,M
        CPI '-'
        JNZ NONNEG
        LDA FLAG
        ORI 01H ; SET LSB TO INDICATE SUBTRACTION LATER
        STA FLAG
        INX H
        JMP PLOOP
        NONNEG:
                LDA FLAG
                ANI FEH ; MASK OUT LSB TO INDICATE NO SUBTRACTION LATER
                STA FLAG
        PLOOP:
                MOV A,M ; GET CHARACTER
                CPI '0'
                JC PLOOPEND
                CPI '9'+1
                JNC PLOOPEND

                CALL MULT16

                MOV A,M ; GET CHARACTER AGAIN, SINCE WE CLOBBERED THE REGISTER
                SUI '0'
                ADD E
                MOV E,A
                MOV A,D
                ACI 00H ; ADD CARRY TO HIGH BYTE (REG B)
                MOV D,A
                
                INX H   ; INCREMENT ADDRESS
                JMP PLOOP

        PLOOPEND:
        LDA FLAG        ; CHECK IF NEGATIVE
        ANI 01H
        CNZ NEGATEDE
        POP B
        RET

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

; CARRIAGE RETURN, LINE FEED TO CONSOLE
TWOCR:  CALL    ENDL           ; DOUBLE SPACE LINES
ENDL:   MVI     A,CR
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
        MVI     M,INBUFSIZE
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
        CALL    ENDL           ; ECHO CARRIAGE RETURN
        LDA     INBUF+2         ; FIRST CHARACTER ONLY
        ANI     01011111B       ; CONVERT LOWER CASE TO UPPER
        CPI     'Y'             ; RETURN WITH ZERO = YES
        RZ
        CPI     'N'             ; NON-ZERO = NO
        JNZ     GETYN           ; ELSE TRY AGAIN
        CPI     0               ; RESET ZERO FLAG
        RET                     ; AND ALL DONE

INBUFSIZE EQU 160
INBUF:  DS      INBUFSIZE+3              ; LINE INPUT BUFFER
FLAG:   DS 1 ; FLAG BYTE FOR USE BY THE PROGRAM
        END