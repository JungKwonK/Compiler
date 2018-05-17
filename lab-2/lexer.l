%{
#include <cerrno>
#include <climits>
#include <cstdlib>
#include <string>

#include "driver.hpp"
#include "parser.h"

// The location of the current token.
static yy::location loc;
%}
%option debug
%option noyywrap nounput batch noinput

%{
  // Code run each time a pattern is matched.
  # define YY_USER_ACTION  loc.columns (yyleng);
%}

%%

%{
  // Code run each time yylex is called.
  loc.step();
%}

[ \t]+					{loc.step();											}
[\n]+					{loc.lines(yyleng); loc.step();							}
"("						{return yy::go_parser::make_LPAREN(loc);				}
")"                     {return yy::go_parser::make_RPAREN(loc);				}
"import"				{return yy::go_parser::make_IMPORT(loc);				}
"package"				{return yy::go_parser::make_PACKAGE(loc);				}
[a-zA-Z_][a-zA-Z0-9_]*	{return yy::go_parser::make_IDENTIFIER(yytext, loc);	}
\"(\\.|[^"])*\"			{return yy::go_parser::make_STRINGLITERAL(yytext, loc);	}
.						{driver.error(loc, "invalid character");				}
<<EOF>>					{return yy::go_parser::make_END(loc);					}
%%


/* open the input file */
void go_driver::scan_begin()
{
	yy_flex_debug = trace_scanning;

	if (file.empty() || file == "-")
	{
		yyin = stdin;
	}
	else if (!(yyin = fopen(file.c_str(), "r")))
	{
		error(file + ": " + strerror(errno));
		exit(EXIT_FAILURE);
	}
}

/* close the input file */
void go_driver::scan_end()
{
	fclose(yyin);
}
