# What is This
This is an assembler for the Altair8800 that I made during my time in the DE CSC215 class at NOVA. It handles the complete instruction set of the Altair8800, as well as comments and custom assembler level pseudo-instructions. The asembler takes in the program as a text file and returns both a `.bin` binary file, which can be loaded into the Altair8800 simulator, and a `.txt` file containing the octal version of the machine code for loading into the physical computer.

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
This project has Node infrastructure for a few simple scripts and managing Prettier. These are mainly development dependencies, but are nice to have in production as well. To check if you have node installed, run `node -v` in the command line. If you still need to install node run the following in the command line:
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

## How to use the Assembler
### Input and Output
All assembly programs should be placed in the `programs` folder within this directory as text files, such as `.txt` files. Once the program runs, the two output files will be generated in the `outputs` folder within this directory. One will be a `.txt` file, which will contain the octal machine code, ready to be used with the physical computer. This machine code should be loaded in starting at memory address zero. The other output file will be a `.bin` file, which can be directly loaded into the simulator by turning on the computer, selecting `Load Binary File`, and selecting the outputted `.bin` file. This will allow the simulator to directly load in your assembled program without having to manually enter it.

### Building the Program
In order to run the program, you must first build it with the `makefile` and Node `package.json` scripts. Simply run `npm run build`, `make`, or `make ALL` to generate the final executable called `.assembler.out`. There will also be build artifacts in the `Dependencies/bin` folder including object (`.o`) files and dependency (`.d`) files. These can be cleaned up by running `npm run clean` or `make clean`.

### Running the Program
To run the program, run either `npm run assemble -- <file_name>` or `./assembler.out <file_name>`, where `<file_name>` is the name of the text file containing your program, which should be placed in the `programs` folder. The program will output the above-mentioned output files if the program is valid or print out the lines that caused errors without making the files.