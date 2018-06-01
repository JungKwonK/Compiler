# Lab 1
This is a top-down recursive descent parser.  

## Build
In the lab-1 folder, run `make`.  
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
PKG_ID       ->	^[a-zA-Z][a-zA-Z0-9]+$
IMP_ID       -> ^([._]|[a-zA-Z][a-zA-Z0-9]+)$
STR_LIT      -> ^"(\\.|[^"])*"$
```

## Testing
To test the program after building it, run `make test`  
Tests are organised as follows:  
```
lab-1s/
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
