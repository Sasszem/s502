# Assembling

The workings of the program can be broken up to 6 steps:
- initialization
- reading of input file
- first processing step
- second processing step
- generating and writing binary data
- quit

## Init
- initializing state variables
- parsing commandline arguments
- loading instructions data

## Reading
- reading input file
- tokenizing
- stripping whitespace and comments from tokens
- recognizing type (directive/instruction/label)
- result is a linked list with tokens
- can be repeated via .include directives

## First step
- label: record position
- instruction: find and link instruction data
- instruction: identify addressmode
- instruction: try parsing operand (but forward-ref labels might be invalid)
- directive: do directive processing
- calculate binsizes
- remove all tokens with binsize=0
- conditional compilation logic
- can repeat reading step via include directive

## Second step
- get token operands (now all should be valid)

## Third step
- generate and concatenate binary data, write it to file
