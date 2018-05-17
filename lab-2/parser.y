
%{
#include "parser.h"
#include "lexer.h"

#include <stdio.h>
#include <unistd.h>


#define YYERROR_VERBOSE 1


int yyerror(const char *msg)
{
  fprintf(stderr, "yyerror: %s\n", msg);
  return 0;
}

extern FILE *yyin;
extern FILE *yyout;
%}

%output "parser.c"
%defines "parser.h"

%define api.pure
//%lex-param {void *scanner}
%parse-param {void *scanner}

%token	END 0
%token	IMPORT "import" PACKAGE "package" IDENTIFIER STRINGLITERAL
%token LPAREN '(' RPAREN ')'

%%

program:		statements;

statements:		statements statement
|				statement;

statement: 		package
| 				imports;

package:		"package" IDENTIFIER;

import:			"import" importitem
|				"import" '(' importitems ')';

imports:		imports import
|				import;

importitem:		STRINGLITERAL
|				IDENTIFIER STRINGLITERAL;

importitems:	importitems importitem
|				importitem;

%%
