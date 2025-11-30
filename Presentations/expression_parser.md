# Brendan and Gabe's PMDAS Expression Parser
*Implementation link: https://github.com/BrendanO123/csc215/blob/main/PMDAS.ASM*

For our final project, Brendan and I wrote a mathematical expression parser in assembly.

Since it would be a pain to explain every single math helper and every single parse loop, this file will give a high level explanation
of the parser's logic. (You can just assume they are all the most brilliant thing you have ever seen).

This is a **broad simplification** of the parser. The real thing is about 900 lines of assembly. You are free to read through that; it can be found on Brendan's repo.

## Evaluation Logic

The main evaluation logic takes **two passes** through the expression.

First pass: 
* multiplication,
* division,
* left shift,
* right shift,
* and modulus.

Second pass: 
* addition
* subtraction.

*Every operand is parsed as a 16-bit signed integer.*

For individual calculations, the program parses the left operand, stores it in DE, parses the right operand, stores in in BC, then jumps to a different math function based on the operator between the two operands. See BONUS - Switch Statement to see how it does that.

After the calculation is finished, the result goes in DE. DE (the result of the calculation) then **replaces the original expression**. This allows for chained operations.

For example, if the expression was `"4*3*2"`, `12` would be stored in place of `"4*3"`, and then `"12*2"` would be calculated. Rinse and repeat until we're finished, then do it again for the addition/subtraction pass.

However, we don't simply insert more ASCII into the original buffer. That would require:

1. Resizing the buffer (Dangerous)
2. Shifting all subsequent characters down a few slots each time (SO SLOW!)
3. Parsing and printing additional strings

Instead, these intermediate calculation results are stored as a binary blob inserted directly *into* the ASCII. We can then continue to parse the expression seamlessly. More on that now...

## Intermediate Values

In our program, every calculation occurs between two operands, has one operator, and yields a single result. So, how do we handle `"4+3+2"`?

First, `4+3` is evaluated in the manner described above. The result (7) is stored back into the string buffer as a 16 bit signed integer. It overwrites `"4+3"` (ASCII) with `7` (a binary number).

However, now we have mixed binary values with ASCII values, which could get messy. Also, what if we have something like `"15x43"`? The 16-bit signed integer will only take up two bytes. But the original expression (`"15x43"`) has **five characters.** Our intermediate value should completely overwrite the expression. 
> Remember, one of the goals of using binary intermediate results was to avoid the slow process of shifting the entire string to the left or the right when inserting a new result. Therefore, the result should take up the same space in the buffer as the original expression. 

So, the intermediate value needs a special structure. which takes advantage of the fact that ASCII is a 7-bit encoding:

```
Offset from P:   0                1 ... (skip-1)         (skip+1)     (skip+2)
                 ┌──────────────┬──────────────────────┬────────────┬────────────┐
Bytes:           │  SKIP BYTE   │    (unused / old     │  VALUE_LO  │  VALUE_HI  │
                 │ (flag+count) │   expression bytes)  │   (E)      │   (D)      │
                 └──────────────┴──────────────────────┴────────────┴────────────┘
```

You'll notice that the "special structure" is really just one thing... a "skip byte." The highest bit of the skip byte is always set.

You know what never sets the highest bit? ASCII. Therefore, when the parser sees the highest bit set, we know a binary value is coming down the pipe.

The rest of the skip byte is a 7 bit unsigned integer that represents **how many garbage bytes we need to skip over.** Our original expression, `4x3`, would have no garbage. Since it is 3 bytes, we can overwrite it perfectly:

1. Skip byte (containing the set highest bit), then `0`.
2. Low byte of integer
3. High byte of integer

However, our other expression, `"15*43"`, would need to have a skip byte of 2, since there would be two extra characters (`'5'` and `'*'`) that the parser needs to skip over to get to the real binary data. 

> This works great for our purposes because it allows us to fit intermediates in the smallest equation we will see, 3 bytes for two numbers and an operator, and still completely covers extremely long expressions. This means that the parser will never have to resize the buffer to put the result in or see some of the original expression peaking out from under the intermediate result and get confused.

After all that work, the parser can just read intermediates as if they were a part of the original expression. 

## The Parentheses Stack

Now we have gotten to the fun part. This whole document, I have been lying to you, as all good educators do. We don't simply run over the **whole string** twice like cavemen. The program keeps a parentheses stack, and evaluates **parts** of the expression twice. Here's how it works:

1. When the parser sees an **open parenthesis**, it pushes that location to the parenthesis stack.
2. When parser sees a **close parentheses**, it pops the last open parenthesis location, and evaluates from there to the current location. Both the multiplication/div/etc pass and the addition/subtraction pass is performed over its length. That entire parenthetical expression is then replaced with a binary intermediate value.
3. We either wrap the entire thing in parenthesis or run a final evaluation to clear up anything outside all of the parenthesis. I will let you know when one of these ideas actually works.

And that's it!

> It's actually quite beautiful... I can't remember who came up with it. Was it me or Brendan? What am I saying?! (Brendan is editing this document now and he distinctly recalls explaining this to Gabe repeatedly). Let the record show that ***Brendan*** came up with it.

Having multiple stacks going on the 8080 (the paren stack and the call stack) is a tiny bit hacky. Instead of just using `DS` to allocate room for one stack, we allocate two separate stacks with two `DS` calls. We also assign a place in memory to store our secondary stack pointer with a label and a `DW` instruction. To use the other stack we:

1. Clear `HL`
2. Use `DAD SP` to get call stack pointer into `HL`
3. Put `HL` somewhere safe
4. Load parentheses stack pointer into `HL`
5. Use `SPHL` to load `HL` into `SP`
6. Make our `PUSH` and `POP` calls
7. Update and save the secondary stack pointer accordingly
8. And restore the call stack

## BONUS - Switch Statement

To make certain things easier, like choosing what operation to perform based on a character, "we" (Brendan) cooked up a simple switch statement implementation.

In Java, a switch statement looks like this:

```java
switch (day) {
            case 1:
                System.out.println("Monday");
                break;
            case 2:
                System.out.println("Tuesday");
                break;
            case 3:
                System.out.println("Wednesday");
                break;
            case 4:
                System.out.println("Thursday");
                break;
            case 5:
                System.out.println("Friday");
                break;
            case 6:
                System.out.println("Saturday");
                break;
            case 7:
                System.out.println("Sunday");
                break;
            default:
                System.out.println("Invalid day number");
                break;
}
```

In assembly, our switch statement looks like this:

```asm
DEFAULT EQU 256 ; if the second byte in the case is non-zero, then we know a single digit character was not entered
    ; therefore we assume this is the default case. To get this in a DW we jut load in 256 as a 16 bit number.
    
MULTPASS: DW '*',EVALMULT,'<',EVALLS,'>',EVALRS,'/',EVALDIV,'%',EVALMOD,'+',EVALSKIP,'-',EVALSKIP,DEFAULT,EVALDEFAULT

...

; Test character already in A
LXI H,MULTPASS
JMP SWITCH
```

And here's the implementation, comments and all:

```asm
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
```

Our "switch statement" is really just cases (single byte) followed by memory locations (two bytes).
You define those and then call or jump to SWITCH.

The "memory locations" after each case byte are called "hooks" because they are machine code.
SWITCH starts at the first case, compares it to the target value, and if the comparison succeeds (`JZ` instruction), then it jumps to the hook.

If the comparison fails, it increments the pointer four times to skip the current case as well as the padding from DW and the two address bytes.

Our switch statement even supports a default case and fallthrough (I am sure everyone will appreciate fallthrough down the line with no annoying bugs)!

---------------------

Well, that's it. I hope you enjoyed.

Brendan's GH: @BrendanO123
My GH: @Archonic944
