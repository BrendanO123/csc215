# What is This
This is an assembler for the Altair8800 that I made during my time in the DE CSC215 class at NOVA. It handles the complete instruction set of the Altair8800, as well as comments and custom assembler level pseudo-instructions. The assembler takes in the program as a text file and returns both a `.bin` binary file, which can be loaded into the Altair8800 simulator, and a `.txt` file containing the octal version of the machine code for loading into the physical computer.

## How to use the Assembler
### Input and Output
All assembly programs should be placed in the `programs` folder within this directory as text files, such as `.txt` files. Once the program runs, the two output files will be generated in the `outputs` folder within this directory. One will be a `.txt` file, which will contain the octal machine code, ready to be used with the physical computer. This machine code should be loaded in starting at memory address zero. The other output file will be a `.bin` file, which can be directly loaded into the simulator by turning on the computer, selecting `Load Binary File`, and selecting the outputted `.bin` file. This will allow the simulator to directly load in your assembled program without having to manually enter it.

### Building the Program
In order to run the program, you must first build it with the `makefile` and/or [Node](#node-and-npm) `package.json` scripts. Simply run `npm run build`, `make`, or `make ALL` to generate the final executable called `.assembler.out`. There will also be build artifacts in the `Dependencies/bin` folder including object (`.o`) files and dependency (`.d`) files. These can be cleaned up by running `npm run clean` or `make clean`.

### Running the Program
To run the program, run either `npm run assemble -- <file_name>` or `./assembler.out <file_name>`, where `<file_name>` is the name of the text file containing your program, which should be placed in the `programs` folder. The program will output the above-mentioned output files if the program is valid or print out the lines that caused errors without making the files.

## How the Assembly Language Works
The assembler breaks up your assembly code into each line, and each line must result in either no machine code added (in the case of pseudo-instructions) or a valid instruction that fits evenly into a number of bytes. To do this, the assembler breaks each line down again into tokens separated by whitespace.

### Token Types
 - **Booleans and Register Pairs**:
    - These are literals are handled through direct text lookup instead of regular expressions. Therefore, they are case-sensitive. 
        - Booleans take up one bit in the opcode and are either true or false. The valid boolean literals are: `true`, `TRUE`, `T`, `false`, `FALSE`, and `F`. Note that these must be either all uppercase or all lowercase, not mixed. Zero and one are also not assumed to be binary literals and will be treated as byte literals if encountered.
        - Register pairs take up two bits in the opcode and correspond to one of four pairings: `BC`, `DE`, `HL`, or `Flags and A`. Register pair literals contain the two register letters (either both uppercase or both lowercase) and optionally a prefix of `rp_` or `RP_`. For example, `bc`, `BC`, `rp_bc`, `rp_BC`, `RP_bc`, and `RP_BC` are the valid ways to denote register pair `BC`. For the `Flags and A` pair, the same rules apply with the letters being `FA`. Additionally, `FlagsA`, `rp_FlagsA`, and `RP_FlagsA` will also decode to this register pair for convenience.
 - **Register Address**:
    - *Regex*: `[rR]?[a-eA-EhHlLmM]`
    - Regular Expression Explanation: matches a register name (`A-E`, `H`, `L`, and `M`) as either uppercase or lowercase and with or without a preceding `r` or `R`. For example, the tokens `rA`, `Rb`, and `C` would all be assumed to be register addresses.
    - Token Explanation: represents a single byte register that can be used as a parameter to ALU operations. The `M` register (memory address) is not actually a physical register and will use the RAM register specified by the memory address stored in the `H` and `L` registers for any operation expecting a register parameter. For example, if registers `H` and `L` contain `0` and `128` respectively, storing to register `M` will instead store to RAM register `128`. You can store to this location or pull from it, but cannot do both at the same time.
 - **Byte Literal**:
    - *Regex*: `-?[0-9]+`
    - Regular Expression Explanation: matches a decimal number without separators that can be either positive or negative.
    - Token Explanation: a decimal number representing a single byte of immediate data. This will be treated as an unsigned character and stored within one byte of data. For example, `-1` will be stored as `255`.
 - **RAM Address**: 
    - *Regex*: `[rR][0-9]+`
    - Regular Expression Explanation: matches one or more decimal digit without separators, prefixed by either `r` or `R` to differentiate it from the byte literal.
    - Token Explanation: a decimal number representing a memory address in the RAM memory. This was recently changed to allow addressing RAM registers past register 255 and takes 2 bytes to store.
 - **Operations**:
    - *Regex*: `[a-zA-Z]+`
    - Regular Expression Explanation: matches an operation name starting consisting of only capitalized or lowercase letters. This regular expression also catches variable names and undefined operations, so the operation name must also exist within the lookup table generated from the `opcodes.txt` file. These can be either all uppercase or all lowercase but cannot be mixed.
    - Token Explanation: a name of a machine code instruction, which is used to index the lookup table of opcodes and parameters. For each instruction, the bits that define the opcode as well as the type of parameters required are stored in a lookup table. These parameters should be specified after the operation name on the same line, delimited by spaces or other whitespace. They must be in the same order as they are in the manual: parameters on the left are earlier farther left in the byte or in lower index bytes than those to the right.
 - **Variables**:
    - *Regex*: `[a-zA-Z_][a-zA-Z0-9-_\.]*`
    - Regular Expression Explanation: matches a variable name starting with either a letter or an underscore, followed by zero or more letters, numbers, hyphens, underscores, or dots.
    - Token Explanation: the name of a variable defined somewhere in the file. These variables must be defined at some point in the file with one of the pseudo-instructions (see below), but can be hoisted under [most circumstances](#important-notes) to be used before they are defined. This variable name is replaced with the token it corresponds to at compilation, and is not a variable in the machine itself. No two variables can have the same name and the name of the variable cannot be an instruction.
- **Pseudo-Instructions**:
    - *Regex*: `\.[a-zA-Z-_]+`
    - Regular Expression Explanation: matches a pseudo-instruction name denoted by a leading period. The name itself must only contain letters, hyphens, or underscores. Importantly, this is a far broader catch than the amount of defined pseudo-instructions, and any undefined instructions will cause in error if used.
    - Token Explanation: an operation that is not translatable to machine code and is strictly an assembler function. All functions have aliases in both camelCase and SCREAMING_SNAKE_CASE. The only currently defined pseudo-instructions are: 
        - `.define` and its aliases `.def`, `.variable`, and `.var`:\
        takes in a variable name (see above) and a token, such as a boolean, register pair, register, byte, memory address, or previously defined—**CANNOT BE HOISTED**—variable. This creates a new variable, so that whenever the variable name is referenced anywhere in the program (these variables have recently been changed to be hoisted), the token is replaced for the variable name like a constant variable. Note that if defining a new variable as equal to an existing variable, the existing variable must be defined above the new variable.
        - `.position` and its aliases `.pos`, `.function`, and `.func`:\
        takes in a variable name the same as `.define`, but does not take in a token to set the variable to. Instead, the variable gets set to the memory address of the next instruction down in the machine code. This can be used to name positions in the code and go to them with call or jump statements without having to deal with brittle, hard-coded addresses. These variables are also hoisted allowing for "functions" to be defined at the end of the program and called within the main execution.
        - `.relJump` and its aliases `.relativeJump`, `.relJmp`, `.offsetJump`, and `.offJmp`:\
        takes in an instruction, memory address or variable representing a memory address, and byte offset (which is treated as a signed integer instead of an unsigned byte). For example `.relJump JMP 15 -3` Adds the instruction to the compiled machine code like normal and computes the sum of the memory address and offset before pushing the address to the compiled machine code. This can be used to jump to a named position with an offset to cut down on named position count.
        - `.initSP` and its aliases `.initStackPointer`, and `.initializeStackPointer`:\
        sets the `Stack Pointer` to the memory address found in the special variable `LAST_ADDRESS` or the default value `TokenProcessor :: estimatedRAMSize` defined in the `TokenProcessor.h` file. The current value for this default is 8191 as the last available RAM register in the simulator. If this function uses the default value, it will define `LAST_ADDRESS` as a variable for the rest of the program to use. This overrides the current address in the `Stack Pointer` and can interfere with stack data, so is only meant to be used at the start of the program before any `CALL`, `PUSH`, or similar instructions. If the `Stack Pointer` is not set, it will default to 0, putting the first pushed data on the stack at the last addressable RAM register. This will work fine if the computer has all 65 KB of RAM, but the simulator for this class does not, so the data will be incorrectly discarded and any subsequent `RET` calls will place the `Program Counter` back at address zero, likely causing an infinite loop.

### Important Notes
***Comments*** are denoted by a `#` as the first non-whitespace character on a line, they will cause the entire line to be ignored by the assembler. *Comments cannot be appended onto the end of a line* of code and must be on their own line. Empty lines will similarly be ignored by the assembler: **empty lines do not generate NOP instructions.**

**Hoisting**: (**NEW**) variable definitions **are hoisted** meaning any variable declared with `.define`, `.position` or similar can be referenced from anywhere in the program, even above its declaration. As an exception to this, a variable cannot be declared with `.define` and set to the value of a variable that is declared latter in the program. Some logic to implement this is in place, so it may be looked into in the future.

**Case Sensitivity**: all lookups in the program are case-sensitive. This means that any variable, register pair, boolean, instruction/operation, or pseudo-instruction must be defined in the given case format. Variables must match the case format defined exactly: they are fully case-sensitive. For instructions, pseudo-instructions, register pairs, and booleans, the name of the token must be typed in either all uppercase or all lowercase, not mixed uppercase and lowercase. For example, `FALSE` and `false` or `.define` and `.DEFINE` will decode to the same things for convenience, but `False` or `.Define` will not compile. The exception to this is the boolean literals `T` and `F`, which will not decode if typed in as lowercase letters.

## Dependencies
### Node and NPM
This project has Node infrastructure for a few simple scripts and Prettier formatting. These are mainly development dependencies, but Node may be nice to have in general. To check if you have node installed, run `node -v` in the command line. If you still need to install node run the following in the command line:
```bash
# Download and install nvm:
curl -o- https://raw.githubusercontent.com/nvm-sh/nvm/v0.40.3/install.sh | bash

# in lieu of restarting the shell
\. "$HOME/.nvm/nvm.sh"

# Download and install Node.js:
nvm install 22

# Verify the Node.js version:
node -v # Should print "v22.19.0".

# Verify npm version:
npm -v # Should print "10.9.3".
```
#### Node and `.zshrc` Files
To keep Node working when you restart Terminal, you need to add the following to your `.zshrc` file or equivalent:
```bash
export NVM_DIR="$HOME/.nvm"
[ -s "$NVM_DIR/nvm.sh" ] && \. "$NVM_DIR/nvm.sh"  # This loads nvm
```

#### Making a `.zshrc` File
If you don't have a `.zshrc` file, you can run the following script to create one with these lines loaded:
```bash
cd ~
touch .zshrc
echo 'export NVM_DIR="$HOME/.nvm"\n[ -s "$NVM_DIR/nvm.sh" ] && \. "$NVM_DIR/nvm.sh"  # This loads nvm' > .zshrc
```
or the one-line version:
```bash
cd ~ && touch .zshrc && echo 'export NVM_DIR="$HOME/.nvm"\n[ -s "$NVM_DIR/nvm.sh" ] && \. "$NVM_DIR/nvm.sh"  # This loads nvm' > .zshrc
```

These files contain bash or z-shell scripts, which are run every time a new terminal window is loaded. These can be used to set environment variables for your terminal, and must be in your users folder at `Users/<your_user_id>`, such as `Users/1006609`. To see these files in explorer, you must toggle on hidden files view with Command + Shift + Period.

### Prettier
This code has Prettier architecture for using Prettier as a linter. This is leftover from when the instruction set was to be in `.json` files, and is not very significant anymore as the linter cannot work with C++. If you already have the linter installed you can run the formatter with `npm run format`, but I would not recommend installing it for this project if you do not already have it.