%skeleton "lalr1.cc"

%defines
%define parser_class_name {go_parser}

%define api.token.constructor
%define api.value.type variant
%define parse.assert

%code requires
{
#include <string>

class go_driver;
}

%{
#include "ast_node.hpp"
%}

/* so that we have a place to store the generated AST */
%param { go_driver& driver }

%locations
%initial-action
{
	@$.begin.filename = @$.end.filename = &driver.file;
};

%define parse.trace
%define parse.error verbose

%code
{
#include "driver.hpp"
}

%define api.token.prefix {TOK_}

%token
	END		0
	IMPORT	"import"
	PACKAGE	"package"
	LPAREN	"("
	RPAREN	")"
;

%token <std::string>	IDENTIFIER		"identifier"
%token <std::string>	STRINGLITERAL	"stringliteral"

%type<ast_node> program statements statement package import imports importitem importitems

%%
%start program;

program:		statements END					{driver.tree = ast_node(ast_root, $1);};

statements:		statements statement			{$$ = ast_node(ast_undefined, $1, $2);}
|				statement						{$$ = $1;};

statement: 		package							{$$ = ast_node(ast_package_statement, $1);}
| 				imports							{$$ = ast_node(ast_import_statement, $1);};

package:		"package" IDENTIFIER			{$$ = ast_node(ast_package_statement, ast_node(ast_string_literal), ast_node(ast_identifier));};

import:			"import" importitem				{$$ = ast_node(ast_import, ast_node(ast_string_literal), $2);}
|				"import" "(" importitems ")"	{$$ = ast_node(ast_import, ast_node(ast_string_literal), $3);};

imports:		imports import					{$$ = ast_node(ast_undefined, $1, $2);}
|				import							{$$ = $1;};

importitem:		STRINGLITERAL					{$$ = ast_node(ast_import_item, ast_node(ast_string_literal));}
|				IDENTIFIER STRINGLITERAL		{$$ = ast_node(ast_import_item, ast_node(ast_identifier), ast_node(ast_string_literal));};

importitems:	importitems importitem			{$$ = ast_node(ast_undefined, $1, $2);}
|				importitem						{$$ = $1;};
%%

void yy::go_parser::error(const location_type& l, const std::string& m)
{
	driver.error(l, m);
}
