%{
	#include <stdio.h>
%}

%%

[ \t\n]                 		;
[a-zA-Z_][a-zA-Z0-9_]*		return IDENTIFIER;
"(\\.|[^"])*"			return STRINGLITERAL;
"("                     		return TOKEN(TLPAREN);
")"                     		return TOKEN(TRPAREN);
"{"                     		return TOKEN(TLBRACE);
"}"                    		return TOKEN(TRBRACE);

%%
