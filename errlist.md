# Errors list

This program can handle many errors:
- file open errors
- memory allocation errors (in most places)
- invalid inputs (not 100% strictly checked, but a huge percentage is covered)

The handled errors do not cause the program to crash, instead, it prints where it encountered the problem, tells details and then de-allocates all used resources before quitting.

Error signaling is based on return values, usually negative return values indicating error. Most functions also print FAIL messages when one of their sub-tasks quitted with an error, thus giving us a partial traceback.

Bellow is a list of all possible errors, and one (or many) ways to reproduce them.   
This list can be used to check if no error ever leaks memory or crashes the program. (which I did by hand)

Please note that the described ways are only examples on how to trigger these errors, and might NOT cover all cases for all errors, but all currently reproducible errors have at least one example.

```c
ERROR("Mismatched number of arguments for '%s'\n", line[0]); // define
```
Reproduce:
- `.define`
- `.define A`
- `.define A B C`

(any number of arguments other than 2)

---
```c
ERROR("Can not use undefined labels with define!\n");
```
Reproduce:
- `.define A &nondef`

---
```c
ERROR("Too long constant name! (max is %d chars)\n", MAP_MAX_KEY_LEN - 1);
```
Reproduce:
- `.define veeeeery_loooooong_coooonstant_naaame 5`

(actual max value is set in map.h)

---
```c
ERROR("Mismatched number of arguments for '%s'\n", line[0]); // ifbeq
```
Reproduce:
- `.ifbeq`
- `.ifbeq A`
- `.ifbeq A B C`

(any number of arguments other than 2)

---
```c
ERROR("Can not forward-ref labels with ifbeq!\n");
```
Reproduce:
- `.ifbeq &A &B`

Triggers on any undefined label

---
```c
ERROR("Print with empty message!\n");
```
Reproduce:
- `.print`

---
```c
ERROR("Mismatched number of arguments for '%s'\n", line[0]); // printc
```
Reproduce:
- `.printc`
- `.printc A B`

(any number of arguments other than 1)

---
```c
ERROR("Mismatched number of arguments for '%s'\n", line[0]); // include
```
Reproduce:
- `.include`
- `.include A B`

(any number of arguments other than 1)

---
```c
ERROR("Mismatched number of arguments for '%s'\n", line[0]); // ifdef / ifndef
```

Reproduce:
- `.ifdef`
- `.ifndef`
- `.ifdef A B`
- `.ifndef A B`

(any number of arguments other than 2)

---
```c
ERROR("Mismatched number of arguments for '%s'\n", line[0]); // org
```
Reproduce:
- `.org`
- `.org A B`

(any number of arguments other than 1)

---
```c
ERROR("Can not use undefined labels with org!\n");
```
Reproduce:
- `.org &nondefined`

---
```c
ERROR("Mismatched number of arguments for '%s'\n", line[0]); // pad
```
Reproduce:
- `.pad`
- `.pad A B C`

(any number of arguments other than 1 or 2)

---
```c
ERROR("Invalid argument in .pad!\n");
```
Reproduce:
- `.pad ABC`
Triggers on any non-defined labels/constants or on number parsing errors   
Might also trigger "Can not interpret number" error

---
```c
ERROR("Negative padding! PC=%x, target=%x\n", s->PC, target);
```

Reproduce:
- `.org $500`   
  `.pad $400`

---
```c
ERROR("Unknown directive: %s\n", directive);
```
Reproduce:
- `.whatisthis`

---
```c
ERROR("Label '%s' is not defined!\n", &arr[i][2]);
```
Reproduce:
- `.data w:&nondefinded`
- `.data &nondefined`

(same message for both words and bytes, but with two different ERROR lines)   
Also triggers "Invalid byte/word in .data" message

---
```c
ERROR("Invalid word in .data!\n");
```
Reproduce:
- `.data $error`

Might also trigger "Can not interpret number" or "Label is not defined"

---
```c
ERROR("Malformed string in .data! (no whitespaces allowed even in quot
es)\n");
```
Reproduce:
- `.data "abc`

---
```c
ERROR("Invalid byte in .data!\n");
```
Reproduce:
- `.data $5 256`
- `.data $5 $error`

Might also trigger "Can not interpret number" or "Label is not defined"

---
```c
ERROR("Can not pad with invalid value!\n");
```
Reproduce:
- `.pad $100`
- `.pad $error`

Triggers if pad byte is >255 or does not have a value (undef label/constant, number parsing error)   
Might also trigger ""Can not interpret number

---
```c
ERROR("An error occured opening the file %s!\n", fname);
ERROR("Error opening file: %s\n", strerror(errno));
```
Reproduce:
- delete/rename/move instructions file
- invalid input file
- invalid output file (no write access, antivirus, etc)
- include a non-existent file

---
```c
ERROR("Malformed instruction line:column: %d : %d\n", row + 1, rowindex +
 1);
ERROR("Malformed instructions/opcodes file: %s\n", fname);
```
Reproduce:
- tinker with instructions file, add a ; for example

---
```c
ERROR("Memory allocation error in instruction_load()!\n");
```
---
```c
ERROR("Memory allocation error in istack_new()!\n");
```
```c
ERROR("Memory allocation error in istack_push()!\n");
```
```c
ERROR("Memory allocation error in map_new()!\n");
```
```c
ERROR("Memory allocation error in map_set()!\n");
```
```c
ERROR("Memory allocation error in state_new()");
```
```c
ERROR("Memory allocation error in tokenslist_new()!\n");
```
```c
ERROR("Memory allocation error in tokenslist_add()!\n");
```
No known ways to reproduce any memory allocation errors

---
```c
ERROR("line is too long!\n");
```
Reproduce:   
- `some terribly (and also unnecessarily) long line that should never exist in an assembly source file, not even in .data directives bc it's unreadable`

Please note that comments and whitespaces does NOT count here

---
```c
ERROR("Can not interpret number: '%.*s'\n", count, str);
```
Reproduce:
- `.data THIS_IS_NOT_A_NUMBER`

Can trigger anywhere where a number was expected but could not be parsed.   
Might also have other error messages.

---
```c
ERROR("More endif's than if's!\n");
```
Reproduce:
- `.endif`

---
```c
ERROR("Label is too long!");
```
Reproduce:
- `this_label_is_so_long_it_will_genererate_an_error:`

Limit is set in map.h

---
```c
ERROR("Can not re-define label '%s'\n", labelname);
ERROR("(prev. value: $%x = %d)\n", map_get(s->labels, labelname), map_get(s->labels, labelname));
```
Reproduce:   
- `my_label:`   
  `my_label:`

---
```c
ERROR("Undefined label reference!\n");
```
Reproduce:
- `lda &undef_label`

Can only happen in instructions

---
```c
ERROR("Argument '%s' excepts parameter(s)!\n", argv[i]);
```
Reproduce:   
- `asm.exe -l`
- `asm.exe -d`
- `asm.exe -d A`
- `asm.exe -o`

---
```c
ERROR("Out parameter is too long! (max: %d chars)\n", STATE_MAX_STRING_LEN - 1);
```
Reproduce:
- `asm.exe -o very/long/file/path/that/wont/fit/in/my/small/buffer`

Maximum can be set in state.h

---
```c
ERROR("Output file name can not start with a dash ('-')!\n");
```
Reproduce:
- `asm.exe -o -argument2`

---
```c
ERROR("Can not parse log level!\n");
```
Reproduce:
- `asm.exe -l ABCD`

---
```c
ERROR("Can not parse value of define '%s': '%s'!\n", name, value);
```
Reproduce:
- `asm.exe -d A ABCD`

---
```c
ERROR("Define constant name is too long! (max: %d chars)\n", MAP_MAX_KEY_LEN - 1);
```
Reproduce:
- `asm.exe -d very_long_constant_name_that_will_cause_an_error 2`

Maximum can be set in map.h
---
```c
ERROR("Input file name is too long! (max: %d chars)\n", STATE_MAX_STRING_LEN - 1);
```
Reproduce:
- `asm.exe very_very_VERY_long_input_file_name_or_path`

Maximum can be set in state.h

---
```c
ERROR("Input file name ('%s') can not start with a dash ('-')!\n", argv[i]);
```
Reproduce:
- `asm.exe -not_an_input_file`

---
```c
ERROR("No input file was given!\n");
```
Reproduce:
- `asm.exe`

---
```c
ERROR("Unknown instruction!\n");
```
Reproduce:
- `wtf #$5`

---
```c
ERROR("Can not determine instruction address mode!\n");
```
Reproduce:
- `lda *#(5),Z`

---
```c
ERROR("Invalid instruction-addressmode combination!\n");
ERROR("A-mode: %s\n", ADRM_NAMES[t->instr.addressmode]);
```
Reproduce:
- `lda ($1234)`

---
```c
ERROR("Can not recognize token:\n");
```
Reproduce:
- `la:`
- `.ab`
- `.label:`

---
```c
ERROR("Something went terribly wrong!\n");
ERROR("A LABEL in last pass!\n");
```
No known ways to reproduce

---
```c
ERROR("Relative addressing jump too far!\n");
```
Reproduce:
- `.org 0`   
`beq $8000`

Max relative jump distance is 8 bit signed ((-128)-127)
