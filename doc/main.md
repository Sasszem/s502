# S502 assmebler

Sixty-502 assember is a simple assembler program for the 6502 line of processors written in C.

It is also my homework project in programming on BME.

**IMPORTANT**   
- [Differences from my previously handed-in documentation](changes.md)   
- [Said (outdated) documentation (in Hungarian)](https://docs.google.com/document/d/1sRYvylJ-WbkH5esaBcsoMkdLBEjIHsnn4SUdWfNLwUM/edit?usp=sharing)

## Introduction

The goal of this program is to convert one or many assembly source files to machine code binary files.
This is a moderately complex task, especially as we need robust error handling as there are many possible sources of failure, such as wrong filenames or invalid inputs.

[Information on the 6502](https://www.masswerk.at/6502/6502_instruction_set.html)

### Usage

The assembler consist of an executable file (`asm.exe` on windows), and an instructions file (`opcodes.csv`). 

The former is the main executable of the program.

The later is used by the assembler to load instruction data from, and should be placed in CWD or in PATH, otherwise the assembler will not find it and fail.

The assembler can be run from the command line: `asm.exe input.asm`

Additional settings can also be applied:
- `-l <loglevel>` to set logging level    
  (>=1 number, default is 1, max useful value is 5)
- `-o <outputfile>` to set output file    
  (default is `out.bin`).    
  `name` has an upper length limit set in `state.h`
- `-d <name> <value>` to define a constant.    
  (name is string with upper length limit set in `map.h`, value is non-negative decimal number).    
  **Can be repeated many times.**

The input file should be [a valid source file](sources.md)

The program [assembles the source(s)](assemble.md) into a binary and saves the result into the specified output file.

If the program encounters some errors, it prints it's cause and stops, de-allocating all resources it has before exiting.

[Full list of errors and possible causes](errlist.md)

### Compilation

The project ships with VSCode project set-up for Windows+gcc, the default build task generates `asm.exe` with `-O3`, `-Wall` and `-Werror`.

If not using VSCode or Windows, use the command:
- `gcc -o asm -O3 -Wall -Werror src/*.c`

### Provided example

Two example files are provided in the root folder:
- `example.asm`
- `c64.asm`

`c64.asm` is a "header" file not generating any binary, but defining useful labels for programming the C=64 computer

`example.asm` is a demo program of the assmebler for the C=64 (using `c64.asm`).

For trying out the example, I recommend using a C=64 emulator such as [VICE](https://vice-emu.sourceforge.io/)

Assemble the example with
- `asm.exe -o example.prg example.asm`

The resulting file is a valid `.prg` file that can be loaded (or just drag&dropped) in the emulator (or even a real C=64)

The demo can also be assembled with different parameters by setting these constants:
- `WAIT` max counter value used in the busy wait loop in the demo. Default value is 3000, setting it lower will make the demo run faster
- `CRAZY_EFFECT` setting it to any value will cause a crazy visual effect in the waiting screen
- `BORDERCHANGE` setting it to any value will cause the screen border to be changed in the demo

## Programming

The project code only contains C source along with header files. All of those are located in the `src` directory.

The sources are split up into multiple C files with multiple headers. Headers are always named same as the corresponding sources except for `token_t.h` which has no associated C source and `debugmalloc.h` [which is a debugging utility not written by me.](https://infoc.eet.bme.hu/debugmalloc/)

(The original `debugmalloc.h` raised a assembler warning (a false positive my program also had), so I added GCC pragmas to suppress that.)

The program uses no global variables and only one static variable (global log level, accessed via logging_level()).

### Conventions

Objects:
- should always use pointers
- constructors should always return pointer or NULL
- destructors (free) should free ALL memory, including object handle   
  (setting it to NULL is the responsibility of the caller)

Naming:
- snake_case
- start with module / class name

Errors:
- functions return 0 or negativa to signal errors
- object creration should return NULL to signal error
- caller should check return values
- most levels that pass an error should print fail message

Commenting:
- doxygen doc comments in headers except locally defined function
- comment complicated or not obvious design

### Data structures

There are many of them, a compleate list of them and their descriptions is seen on the auto-generated pages

#### State
Compiler state, e.g constants, labels, tokens and settings.   
Emulates a global object by being passed to many functions.

#### Map
Key->value map for string keys and non-negative integer values.   
Used for constants and labels. Backed by a singly linked list without sentinels.

#### iStack
Simple stack for non-negative integers.   
Only used to store state of conditional compilation in step_one.c

#### Instruction
Type that stores instruction data (mnemonic, codes of different addressing modes)   
Always bound to a linked list of them

#### Token
Most used data type, gathering all data about a single token    
Holds source (for printing source of error), stripped text, address, binsize, instruction pointer (if instruction), operand, etc.

#### TokensList
A doubly linked list of tokens for storing file(s) contents   
Supports delete and insert operations

### Modules

#### Util

Some utility functions

#### Number

Number parsing

#### Directive

Directive processing (step 1 and 3)

#### Loadfile

File loading and tokenizing

#### pass_one

First step

### pass_twothree

Steps 2 and 3 (i.e writing output)