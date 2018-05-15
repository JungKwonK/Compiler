# Lab 2
A parser and lexer made with Bison and Flex.  
Written in C++.

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
