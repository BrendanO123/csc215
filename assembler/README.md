## How the Assembly Language Works
The assembler breaks up your assembly code into each line, and each line must result in either no machine code added or a valid instruction that fits evenly into a number of bytes. To do this, the assembler breaks each line down again into tokens separated by whitespace. 

### Token Types
 - **RAM Address**: 
    - *Regex*: `[0-9]+`
    - Regular Expression Explanation: matches one or more decimal digit without separators.
    - Token Explanation: a decimal number representing a memory address in the RAM memory. Must fit within **1 byte**, but **takes two full bytes of memory to store** because the other byte must be all zeros.
- **Register Address**:
    - *Regex*: `[rR]?[a-eA-EhlmHLM]`
    - Regular Expression Explanation: matches a register name (`A-E`, `H`, `L`, and `M`) as either uppercase or lowercase and with or without a preceding `r` or `R`. For example, the tokens `rA`, `Rb`, and `C` would all be assumed to be register addresses.
    - Token Explanation: a single byte register that can be used as a parameter to ALU operations.
- **Register Pairs and Booleans**:
    - These are handled through direct text lookup instead of regular expressions due to being added later in development. They, therefore, are also more case-sensitive than memory addresses or register addresses. 
        - Booleans take up one bit in the opcode and are either true or false. The valid boolean literals are: `true`, `TRUE`, `T`, `false`, `FALSE`, and `F`. Note that these must be either all uppercase or all lowercase, not mixed. Zero and one are also not assumed to be binary literals and will be treated as memory addresses if encountered.
        - Register pairs take up two bits in the opcode and correspond to one of four pairings: `BC`, `DE`, `HL`, or `Flags and A`. Register pair literals contain the two register letters (either both uppercase or both lowercase) and optionally a prefix or `rp_` or `RP_`. For example, `bc`, `BC`, `rp_bc`, `rp_BC`, `RP_bc`, and `RP_BC` are the valid ways to denote register pair `BC`. For the `Flags and A` pair, the same rules apply with the letters being `FA`. Additionally, `FlagsA`, `rp_FlagsA`, and `RP_FlagsA` will also decode to this register pair.
- **Operations**:
    - *Regex*: `[a-zA-Z_][a-zA-Z0-9-_\.]*`
    - Regular Expression Explanation: matches an operation or variable name starting with either a letter or an underscore, followed by zero or more letters, numbers, hyphens, underscores, or dots. This regular expression also catches variable names and undefined operations, so the operation name must also exist within the lookup table generated from the `opcodes.txt` file. These can be either all uppercase or all lowercase but cannot be mixed.
    - Token Explanation: a name of a machine code instruction, which is used to index the lookup table of opcodes. Each opcode contains the defining bits of the code as well as how many other bits within the first byte are reserved for parameters such as booleans, registers, or register pairs. These parameters should be specified after the operation name on the same line, delimited by spaces or other whitespace. For operations where the parameters are in separate bytes, such as operations that rely on RAM memory addresses, the parameters do not strictly need to be on the same line as the operation name, but it is encouraged to do so.
- **Pseudo-Instructions**:
    - *Regex*: `\.[a-zA-Z-_]+`
    - Regular Expression Explanation: matches a pseudo-instruction name denoted by a leading period. The name itself must only contain letters, hyphens, or underscores. Importantly, this is a far broader catch than the amount of defined pseudo-instructions, and any undefined instructions will cause in error if used.
    - Token Explanation: an operation that is not translatable to machine code and is strictly an assembler function. The only currently defined pseudo-instructions are: 
        - `.define` and its aliases `.def`, `.variable`, and `.var`:\
        takes in a variable name, which must obey the same rules as the operation name and match the same regex, and a token, such as a memory address, register, register pair, or boolean. Then, whenever the variable name is referenced later down in the program (these variables are not hoisted), the token is replaced for the variable name like a constant variable.
        - `.position` and its aliases `.pos`, `.function`, and `.func`. `.define`:\
        takes in a variable name the same as `.define`, but does not take in a token to set the variable to. Instead, the variable gets set to the memory address of the next instruction down in the machine code. This can be used to name positions in the code and go to them with call or jump statements without having to deal with brittle, hard-coded addresses.

### Important Notes
***Comments*** are denoted by a `#` as the first non-whitespace character on a line, they will cause the entire line to be ignored by the assembler. *Comments cannot be appended onto the end of a line* of code and must be on their own line. Empty lines will similarly be ignored by the assembler: **empty lines do not generate NOP instructions.**

**Hoisting**: variable definitions are not hoisted meaning any variable declared with `.define`, `.position` or similar must be declared before it can be used. Note this means that you cannot use named positions to jump forwards in the program. Adding relative offsets to named positions jumps on an assembler level may be looked into in the future, but for now you must make do with return calls and similar logic.

**Case Sensitivity**: all lookups in the program are case-sensitive. This means that any variable, register pair, boolean, instruction/operation, or pseudo-instruction must be defined in the given case format. For variables, the given case format must match the one defined: they are fully case-sensitive. For instructions, pseudo-instructions, register pairs, and booleans, the name of the token must be typed in either all uppercase or all lowercase, not mixed uppercase and lowercase. However, `FALSE` and `false` or `.define` and `.DEFINE` will decode to the same things for convenience. The exception to this is the boolean literals `T` and `F`, which will not decode if typed in as lowercase letters.

## Dependencies
### Node and NPM

#### Node and `.zshrc` Files

## How to use the Assembler
### Input and Output

### Running the Program

### Scripts

### How to use the Output
