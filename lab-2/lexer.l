%{
#include "parser.h"

#include <stdio.h>
%}

%option outfile="lexer.c" header-file="lexer.h"
%option warn nodefault
%option reentrant noyywrap never-interactive
%option bison-bridge

%%

[ \t\n]                 	{}
"("                     	{return LPAREN;}
")"                     	{return RPAREN;}
"import"					{return IMPORT;}
"package"					{return PACKAGE;}
[a-zA-Z_][a-zA-Z0-9_]*		{return IDENTIFIER;}
\"(\\.|[^"])*\"				{return STRINGLITERAL;}

%%

int yyerror(const char *msg)
{
  fprintf(stderr, "yyerror: %s\n", msg);
  return 0;
}
