# Lab 2
A parser and lexer made with Bison and Flex.  
Written in C++.

## Build
In the lab-2 folder, run `make`.  
## Usage
``` bash
./parser input.txt
```
_or_

``` bash
cat input.txt | ./parser
```

## Grammar
This parser recognises a subset of the Go programming language.  

**Non-terminals:**  
```
PROGRAM      -> STATEMENTS

STATEMENTS   -> STATEMENTS STATEMENT
             |  STATEMENT

STATEMENT    -> PACKAGE
             |  IMPORT

PACKAGE      -> "package" IDENTIFIER

IMPORT       -> "import" IMPORTITEM
             | "import" '(' IMPORTITEMS ')'

IMPORTS      -> IMPORTS IMPORT
             | IMPORT

IMPORTITEM   -> STRINGLITERAL
             |  IDENTIFIER STRINGLITERAL

IMPORTITEMS  -> IMPORTITEMS IMPORTITEM
             | IMPORTITEM
```

**Terminals:**  
```
IDENTIFIER       ->	^[a-zA-Z_][a-zA-Z0-9_]*$
STRINGLITERAL    -> ^"(\\.|[^"])*"$
```

## Testing
To test the program after building it, run `make test`  
Tests are organised as follows:  
```
lab-2/
|--
    |-- test/
        |-- mytest/
            |-- input.txt
            |-- output.txt
        |-- another_test/
            |-- input.txt
            |-- output.txt

        ...and so on.
```

The test directories can have any name.  
`input.txt` is the input to the parser.  
`output.txt` is the expected output from the parser.  

Test results are summarised in the terminal output.  
Full results are found in `test/output.log`.  
