# Lab 3
**Note:** *The work for this lab is unfinished.*  
  
A compiler that uses Flex, Bison and LLVM.  

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
program:        pkg_decl imp_decls stmts
|               pkg_decl stmts

stmts:          stmt stmts
|               stmt

stmt:           func_decl
|               var_decl
|               expr

pkg_decl:       "package" ident

imp_decls:      imp_decl imp_decls
|               imp_decl

imp_decl:       "import" imp_spec
|               "import" "(" imp_specs ")"

imp_specs:      imp_spec imp_specs
|               imp_spec

imp_spec:       str_lit
|               ident str_lit

func_decl:      "func" ident func_sig block

func_sig:       "(" func_decl_args ")" ident
|               "(" func_decl_args ")"
|               "(" ")" ident
|               "(" ")"

block:          "{" stmts "}"
|               "{" "}"

func_decl_args: func_decl_arg "," func_decl_args
|               func_decl_arg

func_decl_arg:  ident ident

func_call_args: expr "," func_call_args
|               expr

var_decl:       "var" var_spec

var_spec:       ident ident
|               ident "=" expr
|               ident ident "=" expr

expr:           ident "=" expr
|               ident "(" func_call_args ")"
|               ident "(" ")"
|               ident
|               int_lit
|               "(" expr ")"
|               expr binary_op expr

binary_op:      "+"
|               "-"
|               "*"
|               "/"

ident:          IDENTIFIER

str_lit:        STRINGLITERAL

int_lit:        INTEGERLITERAL

```

**Terminals:**  
```
IDENTIFIER       -> [a-zA-Z_][a-zA-Z0-9_]*
STRINGLITERAL    -> "(\\.|[^"])*"
INTEGERLITERAL   -> [0-9]+
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
