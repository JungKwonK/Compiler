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

[ \t]+					loc.step();
[\n]+					loc.lines(yyleng); loc.step();

"("						return yy::go_parser::make_LPAREN(loc);
")"                     return yy::go_parser::make_RPAREN(loc);
"{"                     return yy::go_parser::make_LBRACE(loc);
"}"                     return yy::go_parser::make_RBRACE(loc);
","                     return yy::go_parser::make_COMMA(loc);

"="                     return yy::go_parser::make_EQUAL(loc);

"+"						return yy::go_parser::make_PLUS(loc);
"-"						return yy::go_parser::make_MINUS(loc);
"*"						return yy::go_parser::make_MUL(loc);
"/"						return yy::go_parser::make_DIV(loc);

"import"				return yy::go_parser::make_IMPORT(loc);
"func"					return yy::go_parser::make_FUNC(loc);
"package"				return yy::go_parser::make_PACKAGE(loc);
"var"					return yy::go_parser::make_VAR(loc);

[a-zA-Z_][a-zA-Z0-9_]*	return yy::go_parser::make_IDENTIFIER(yytext, loc);
[0-9]+					return yy::go_parser::make_INTEGERLITERAL(yytext, loc);
\"(\\.|[^"])*\"			return yy::go_parser::make_STRINGLITERAL(yytext, loc);

<<EOF>>					return yy::go_parser::make_END(loc);
.						driver.error(loc, "unknown token");

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
