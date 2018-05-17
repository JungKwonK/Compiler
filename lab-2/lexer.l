%{
	#include <stdio.h>
%}

%%

[ \t\n]                 	;
[a-zA-Z_][a-zA-Z0-9_]*		return IDENTIFIER;
\"(\\.|[^"])*\"				return STRINGLITERAL;
"("                     	return LPAREN;
")"                     	return RPAREN;
"import"					return IMPORT;
"package"					return PACKAGE;

%%
