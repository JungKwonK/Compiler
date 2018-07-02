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

ast_node *root = &driver.tree;
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
	END			0
	IMPORT		"import"
	PACKAGE		"package"
	FUNC		"func"
	VAR			"var"
	LPAREN		"("
	RPAREN		")"
	LBRACE		"{"
	RBRACE		"}"
	COMMA		","
	EQUAL		"="
	PLUS		"+"
	MINUS		"-"
	MUL			"*"
	DIV			"/"
;

%token <std::string>
	IDENTIFIER
	STRINGLITERAL
	INTEGERLITERAL
;

%type <ast_root> 		program;
%type <ast_ident> 		ident;
%type <ast_str_lit> 	str_lit;
%type <ast_int_lit> 	int_lit;
%type <ast_block> 		block;
%type <ast_stmt> 		stmts stmt var_decl func_decl;
%type <ast_expr> 		expr func_call_args
%type <ast_pkg_decl>	pkg_decl
%type <ast_imp_decl>	imp_decls imp_decl
%type <ast_imp_spec>	imp_specs imp_spec
%type <ast_func_sig>	func_sig
%type <ast_var_decl>	var_spec func_decl_args func_decl_arg
%type <std::string>		binary_op
;

%left ","

%right "="

%left "+" "-"
%left "*" "/"

%left "("

%%

%start program;

program:		pkg_decl imp_decls stmts			{root = ast_root($1, $2, $3);}
|				pkg_decl stmts						{root = ast_root($1, $2);};

stmts:			stmt stmts							{$$ = $1; $$.setNext($2);}
|				stmt								{$$ = $1;};

stmt:			func_decl							{$$ = $1;}
|				var_decl							{$$ = $1;}
|				expr								{$$ = $1;};

pkg_decl:		"package" ident						{$$ = ast_pkg_decl($2);};

imp_decls:		imp_decl imp_decls					{$$ = $1; $$.setNext($2);}
|				imp_decl							{$$ = $1;};

imp_decl:		"import" imp_spec					{$$ = $2;}
|				"import" "(" imp_specs ")"			{$$ = ast_imp_decl($3);};

imp_specs:		imp_spec imp_specs					{$$ = $1; $$.setNext($2);}
|				imp_spec							{$$ = $1;};

imp_spec:		str_lit								{$$ = ast_imp_spec($1);}
|				ident str_lit						{$$ = ast_imp_spec($1, $2);};

func_decl:		"func" ident func_sig block			{$$ = ast_func_decl($2, $3, $4);};

func_sig:		"(" func_decl_args ")" ident		{$$ = ast_func_sig($2, $4);}
|				"(" func_decl_args ")"				{$$ = ast_func_sig($2);}
|				"(" ")" ident						{$$ = ast_func_sig($3);}
|				"(" ")"								{$$ = ast_func_sig();};

block:			"{" stmts "}"						{$$ = ast_block($2);}
|				"{" "}"								{$$ = ast_block();};

func_decl_args:	func_decl_arg "," func_decl_args	{$$ = $1; $$.setNext($3);}
|				func_decl_arg						{$$ = $1;};

func_decl_arg:	ident ident							{$$ = ast_var_decl($1, $2);};

func_call_args:	expr "," func_call_args				{$$ = $1; $$.setNext($3);}
|				expr								{$$ = $1;};

var_decl:		"var" var_spec						{$$ = $2};

var_spec:		ident ident							{$$ = ast_var_decl($1, $2);}
|				ident "=" expr						{$$ = ast_var_decl($1, $3);}
|				ident ident "=" expr				{$$ = ast_var_decl($1, $2, $4);};

expr:			ident "=" expr						{$$ = ast_var_assign($1, $3);}
|				ident "(" func_call_args ")"		{$$ = ast_func_call($1, $3);}
|				ident "(" ")"						{$$ = ast_func_call($1);}
|				ident								{$$ = $1;}
|				int_lit								{$$ = $1;}
|				"(" expr ")"						{$$ = $2;}
|				expr binary_op expr					{$$ = ast_operation($1, $2, $3)};

binary_op:		"+"									{$$ = "+";}
|				"-"									{$$ = "-";}
|				"*"									{$$ = "*";}
|				"/"									{$$ = "/";};

ident:			IDENTIFIER							{$$ = ast_ident($1);};

str_lit:		STRINGLITERAL						{$$ = ast_str_lit($1);};

int_lit:		INTEGERLITERAL						{$$ = ast_int_lit($1);};

%%

void yy::go_parser::error(const location_type& l, const std::string& m)
{
	driver.error(l, m);
}
