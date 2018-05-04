# Compiler
This is our project for Compiler Construction.  
So far it consists of a top-down recursive descent parser.  

## Build
In the project's top-level directory, run `make`.  
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
ROOT         -> PKG_STMT
             |  PKG_STMT IMP_STMT_REC

PKG_STMT     -> package PKG_ID

IMP_STMT_REC ->	IMP_STMT IMP_STMT_REC
             |  IMP_STMT

IMP_STMT     -> import IMP_ID STR_LIT
             |  import IMP_PATH
             |  import ( IMP_REC )

IMP_REC      -> IMP_ID STR_LIT IMP_REC
             |  IMP_ID STR_LIT
             |  IMP_PATH
```

**Terminals:**  
```
PKG_ID       ->	^[a-zA-Z0-9]+$
IMP_ID       -> ^([._]|[a-zA-Z0-9]+)$
STR_LIT      -> ^".*"$
```

## Testing
To test the project run `make test`  
Tests are organised as follows:  
```
(project root)
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
