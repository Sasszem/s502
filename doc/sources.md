# Source files

**Source files** consist of valid **tokens** and **comments**

## COMMENTS
**comments** start with a semicolon (;) and end at the end of the line
Comments are fully ignored by the assembler.

## TOKENS
**tokens** are a string of non-whitespace non-semicolon characters in a line
They can be surrounded by any number of whitespace and can be followed by a comment in the same line, but no line can contain two tokens.

Tokens must be of 3 types:
- **directive**
- **label**
- **instruction**

## labels*
**Labels** are aliases for addresses in memory. They automatically mark the next address where they are defined, but they do not take any space in the resulting binary.
**Label tokens** must end with a colon (:)

## directives
**directives** are commands for the assembler. They might or might not take space in the binary.   
**directive tokens** must start with a dot (.)   
Valid commands:
- `define`
- `ifdef`
- `printc`
- `print`
- `include`
- `endif`
- `ifndef`
- `ifbeq`
- `org`
- `data`
- `pad`

The assembler is not case-sensitive with directive names

### define &lt;name> &lt;value>
Define a constant with name and value.

### ifdef &lt;constant>
Conditional compilation until the matching `.endif` if the constant is defined.   
Node: does not need @ in front of the name!

### printc &lt;constant>
Log the name and value of a constant.   
Note: does not need @ in front of the constant name!

### print &lt;line>
Logs the rest of the line

### include &lt;file>
Include another file in the source.   
Basically automated copy-paste.   
Searches the file in CWD.

### endif
Close a block of conditional compilation

### ifndef &lt;constant>
Inverted ifdef

### ifbeq &lt;first> &lt;second>
Conditional compilation: first >= second   
Both can be numbers!

### org &lt;number>
Set PC to number

### data &lt;entry> [...]
Can have many entries of these 3 types:
- byte: `&lt;number>` (raises an error if it can't fit in a single byte)
- word: `w:&lt;number>` (gets encoded in $LLHH big-endian format)
- string: `"&lt;str>"` (must not include whitespace)
The binary data from these entries will be concatenated an added in the result

### pad &lt;to> &lt;with>
Pad remaining space to `to` with byte `with`

## instructions
**instructions** are actual 6502 instructions

They must consist of a valid instruction mnemonic (3 letters), followed by an additional whitespace and argument   
(i.e they must end after the mnemonic, or have a space and an argument)

See [this page](https://www.masswerk.at/6502/6502_instruction_set.htm) for a list of valid mnemonics, arguments and their combinations.

Differences to that page:
- this assembler uses star (*) to indicate zeropage (and indexed zeropage) addressing 
- instructions with relative addressing modes take memory addresses instead of 8 bit addresses and the offsets are calculated by the assembler 

The assembler is not case-sensitive with instruction mnemonics or operands.

## Numbers
Numbers in directives and operands can take 4 main forms:
- labels addresses: `&&lt;labelname>`
- defined constants: `@&lt;constant>`
- numbers in hex: `$&lt;number>`
- numbers in decimal: `&lt;number>`

The assembler is not case sensitive with hex numbers (a-f, A-F), but IS case sensitive with constant and label names.

Both constant and label names have an upper limit set in map.h

All types can have 2 additional modifiers:
- `&lt;` to extract low byte (lower 8 bits)
- `>` to extract high byte (higher 8 bits)

When an operand or a directive takes a 16 bit value, it will get stored as the 6502's format (high endian, $LLHH).

Labels can also be forward-referenced in 2 cases:
- instruction operand
- .data entry

