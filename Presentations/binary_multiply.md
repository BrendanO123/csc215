# Binary Multiplication

## What the Program Does
This program takes an array of numbers with arbitrary length from the RAM memory after the instruction code and multiplies them together, storing the result after the parameters in RAM. To do this, it defines a multiplication function that takes in two numbers in the `A` and `E` registers and multiplies them together, storing the result in the `HL` register pair. This function is then called repeatedly throughout the function. The main program execution repeatedly loads in two new numbers from RAM, and tests if the first number is zero. If the number is zero, the program exits the loop assuming the end of the array has been found. If not, it multiplies the two numbers together and pushes the result to the stack. Once the loop has finished, it pops all the products off the stacks and places them in RAM after the data it used in the execution.

## Initialization

This section sets up the stack pointer, data pointer, and product count variables. The stack pointer is a hardware level 16-bit register, which keeps track of where in RAM the next byte pushed to the stack should be placed. This needs to be manually initialized to prevent it from defaulting to RAM address 65k, which is not available in the limited storage on the simulator. The data pointer is a variable I am using to keep track of where in RAM the next two numbers to multiply together can be found. Finally, the product count is a counter I am using to keep track of how many products were generated, in order to move the right number of bytes off of the stack and into the new RAM location when finished.

| ADDR (oct)| MNEMONIC    | OCTAL BYTES       | EXPLANATION                                   |
|----------:|-------------|-------------------|-----------------------------------------------|
|      000  | LXI HL, r8191 | 041 377 037     | Load a pointer to the last RAM address into the `HL` registers |
|      003  | SPHL        | 371               | Set the stack pointer to this address         |
|      004  | LXI HL, 102 | 041 102 000       | Load a pointer to the start of the program's data into the `HL` registers |
|      007  | MVI C, 0    | 016 000           | Set register `C` to zero                      |

## The Main Loop

As said above, the main program loop repeatedly loads in two new numbers from RAM, and tests if the first number is zero. If the number is zero, the program exits the loop assuming the end of the array has been found. If not, it multiplies the two numbers together and pushes the result to the stack. Once the loop has finished, it pops all the products off the stacks and places them in RAM after the data it used in the execution.

| ADDR (oct)| MNEMONIC    | OCTAL BYTES       | EXPLANATION                                   |
|----------:|-------------|-------------------|-----------------------------------------------|
|      011  | MOV A, M    | 176               | Load the accumulator from the memory that the `HL` registers point to |
|      012  | INX HL      | 043               | Increment the `HL` registers to point to the next address |
|      013  | MOV E, M    | 136               | Load the `E` register from the memory that the `HL` registers point to |
|      014  | INX HL      | 043               | Increment the `HL` registers to point to the next address |
|      015  | ORA A       | 267               | Logical OR the accumulator against itself (can set the Zero flag) |
|      016  | JZ 034      | 312 034 000       | If the accumulator is zero, jump out of the loop |
|      021  | INR C       | 014               | Increment the product count                   |
|      022  | PUSH HL     | 345               | Push the data pointer in `HL` to the stack for the next iteration |
|      023  | CALL 055    | 315 055 000       | Call the multiplication function              |
|      026  | POP DE      | 321               | Pop the data pointer into the `DE` registers  |
|      027  | PUSH HL     | 345               | Push the product to the stack (product is stored in `HL` registers) |
|      030  | XCHG        | 353               | Move the data pointer to `HL` registers       |
|      031  | JMP         | 303 011 000       | Jump back to the top of the loop at 011       |

## Multiplication Function

| ADDR (oct)| MNEMONIC    | OCTAL BYTES       | EXPLANATION                                   |
|----------:|-------------|-------------------|-----------------------------------------------|
|      055  | MVI D,0     | 026 000           | Clear D (high byte of DE = 0)                 |
|      057  | LXI H,0     | 041 000 000       | Clear HL (product accumulator)                |
|      062  | MVI B,8     | 006 010           | Loop counter B = 8 (process 8 bits)           |
|      064  | RAR         | 037               | Rotate A right through Carry (get multiplier LSB) |
|      065  | JNC 071     | 322 071 000       | If Carry clear, jump to first XCHG at 071     |
|      070  | DAD DE      | 031               | (Executed only if Carry = 1) HL = HL + DE     |
|      071  | XCHG        | 353               | Swap HL <-> DE                                |
|      072  | DAD HL      | 051               | DE = DE + HL  (effectively shift DE left)     |
|      073  | XCHG        | 353               | Swap back HL <-> DE                           |
|      074  | DCR B       | 005               | Decrement loop counter                        |
|      075  | JNZ 064     | 302 064 000       | If B != 0, repeat loop (back to RAR at 064)   |
|      080  | RAR         | 037               | Rotate A right through Carry to reset multiplier |
|      081  | RET         | 311               | Return from function call to normal execution |

## The Cleanup
After the main loop finishes, the products of the data are stored in reverse order in the last RAM addresses available in the simulator. In order to make the products more usable, they are moved off of the stack and into new RAM addresses after the program's data. This also resets the stack pointer, leaving the full stack available for more program execution.


| ADDR (oct)| MNEMONIC    | OCTAL BYTES       | EXPLANATION                                   |
|----------:|-------------|-------------------|-----------------------------------------------|
|      034  | MVI B, 0    | 006 000           | Clear the `B` register to add the `BC` registers to the data pointer |
|      036  | DAD BC      | 011               | Increment the data pointer in the `HL` registers by 2x the product count, `BC` |
|      037  | DAD BC      | 011               | This causes them to point to the end of the RAM for the products to fix the reverse order |
|      040  | DCR C       | 015               | Decrement the `C` register as part of the for-loop |
|      041  | JM 054      | 372 054 000       | If negative, exit the loop. The same as the C++ for loop: `for(c=count; c>=0; c--)` |
|      044  | POP DE      | 321               | Pop a product off the stack into the `DE` registers |
|      045  | MOV M D     | 162               | Store the most significant byte to the RAM address at the data pointer |
|      046  | DCX HL      | 053               | Decrement the data pointer                    |
|      047  | MOV M E     | 163               | Store the least significant byte to the RAM address at the data pointer |
|      050  | DCX HL      | 053               | Decrement the data pointer                    |
|      051  | JMP 040     | 303 040 000       | Jump to the top of the loop at 040            |
|      054  | HLT         | 166               | Once done, halt the execution to prevent infinite loops |

## Multiplication Function Explanation

1. The **multiplier** (8 bits) is stored in the `A` register.
2. The **multiplicand** (8 bits) is stored in the `E` register (low byte). It is also storing a byte of 0 at the `D` register (high byte). This is because we need extra empty space for all the left shifting we're going to do.
3. The H and L registers are cleared to initialize the partial product. This is where we will accumulate our final answer!
4. The **iteration count** starts at 8. It is in register `B`. It will be decremented every time we move to the next bit of the multiplier. **When the iteration count is 0, the program will end; the mechanics of that will be explained later.**
5. We start by consuming the rightmost bit of our multiplier. This is done using the RAR instruction; it's basically a right shift, but instead of the bit at the end disappearing, it replaces the Carry bit. The carry bit is very important for the control flow of the program.
6. If the Carry bit is NOT set, meaning that the rightmost bit that we just consumed was not on, we skip the next step through a JNC (jump-no-carry) instruction.
7. If the Carry bit IS set, though... that means we need to add the multiplicand to the product. This is accomplished using the DAD (double add) instruction, which adds a certain register pair to `HL`. In this case, we target the `DE` registers since that's where our multiplicand is.
8. Now we need to think about moving on to the next bit in our number. It is two times the last bit. So instead of adding 1 times the multiplicand, we need to add 2 times the multiplicand since the second bit represents a 2. In order to accomplish this, we multiply the multiplicand by 2. Multiplication is communitive; it doesn't matter if we do `(2 * current multiplier bit) * multiplicand` or `(2 * multiplicand) * current multiplier bit` What we just did is equivalent to adding a zero to the end of the number in pen-and-paper multiplication when you move on the to the next digit.
9. The doubling from last step is accomplished using a DAD instruction. We have to XCHG (exchange `DE` with `HL`) first, though, since you are only allowed to DAD to HL. This puts the multiplicand in HL, then we double it by adding HL to itself, and then we swap back using another XCHG.
10. Finally, we decrement register `B` (our loop counter) by 1. This also updates the Zero flag...
11. If the Zero flag is not set (meaning we still have more bits to consume), then we jump back to the beginning of the loop. This functions the same as a C++ for loop, counting down from 8.
12. Finally, we rotate the `A` register once more to reset the multiplier and return with the RET instruction. The RET function jumps to the last memory address pushed to the stack. This works with the CALL instruction, which automatically pushes the memory address to return to onto the stack, to return to the normal execution flow after the CALL instruction. This is a powerful tool for having non-inlined function calls multiple places in the code without having to worry about manually setting a return address.

## New Instructions Links

- [CALL](https://ubuntourist.codeberg.page/Altair-8800/part-4.html#call)
- [RET](https://ubuntourist.codeberg.page/Altair-8800/part-4.html#ret)
- [SPHL](https://ubuntourist.codeberg.page/Altair-8800/part-4.html#sphl)
- [ORA](https://ubuntourist.codeberg.page/Altair-8800/part-4.html#ora)
- [PUSH](https://ubuntourist.codeberg.page/Altair-8800/part-4.html#push)
- [POP](https://ubuntourist.codeberg.page/Altair-8800/part-4.html#pop)


## Assembly Code

See also: [assembler README](../assembler/README.md).
```
# initialization
LXI HL r8191 #set stack pointer
SPHL
LXI HL data #load pointer to first byte of data into HL regs for MOV instruction memory references
MVI C 0 #initialize loop counter (used to determine how many outputs to pop off the stack and move into nearby memory)

# load next numbers to multiply
.pos start
MOV A M
INX HL
MOV E M
INX HL

# if multiplier is zero, assume we have found the end of the numbers to multiply
ORA A
JZ cleanUp

# else increment product count
INR C

PUSH HL #push pointer to next number pair to stack bc registers are wiped by mult func
CALL mult

# push product and retrieve pointer 
POP DE
PUSH HL
XCHG

JMP start #loop


# to make the products easier to find, I am going to move them from the end of RAM to after the data for the program
# I am also going to fix the order so that it lines up with the order of the data

# to start move pointer to end of memory used to store product by incrementing by 2 bytes per product * product count
.func cleanUp
MVI B 0
DAD BC
DAD BC

.pos moveAnswersLoop
DCR C #for (c=product count; c>=0; c--)
JM end

# move two bytes from stack to RAM at pointer and decrement pointer by two
POP DE
MOV M D
DCX HL 
MOV M E
DCX HL

JMP moveAnswersLoop

.pos end
HLT


# multiplication function
.func mult
# clear registers
MVI D 0
LXI HL r0

# init counter
MVI B 8

# for loop
.pos loop

# test LSB of multiplier
RAR
JNC next
# add multiplicand to partial product if one
DAD DE

.pos next
# LShift multiplicand
XCHG 
DAD HL
XCHG

# repeat for loop while counter is > 0 or return when not
DCR B
JNZ loop

RAR
RET


# data for the program
# this can be a list of any length (within 255) of number pairs, which will be multiplied together and placed after the data 
.pos data

# program only terminates when 0 * x is detected
r0

# this is where the result will be stored
# the first (most significant) byte of the first product will be two bytes after the last non-zero number pair
```