# Changes from previous doc

As the project evolved, many first ideas were changed. Here is a list of changes from the previous documentations:

## Dropped .incbin directive

This directive *might* be useful, but implementing it with the constraints of the homework (can not check size of file) would be over-complicated, unrealistic and possibly ugly, so I dropped this feature and focused on other aspects of the program instead.

## Using labels

In the original doc I used the percentage ('%') character with labels as arguments, but the ampersand ('&') character is easier to remember as "the address of a label", so I changed this character.

## Dropped/changed word modifier (!)

Since forcing a number to be 16 bit instead of 8 only made sense in address modes (absolute vs. zpg) and .data directives, and I used a different marking for the former (start character to force ZPG), I also changed the way to .data words via prefixing them with "w:", instead of a general number modifier.

## Removed fields from Token type

While refactoring around Token, non-instruction data entries (and thus the whole union) become useless and thus were removed. Now it has instruction related data or (mostly) uninitialized data if it's not an instruction

## Added 'address' field to Token

Added address field to store the address of the instruction. Used for relative addressing.

## Token member functions

- added `token_recognize` (read step)
- added `token_analyze_instruction`
- added `token_get_operand`
- added `token_compile`
- made `link_instruction` private
- made `get_addressmode` private

See Token for current members and methods!

## Make map_find private

Since it's only used internally by map_set and map_get, I removed this function from the private interface, but kept it for internal use.