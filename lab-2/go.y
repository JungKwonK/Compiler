%skeleton "parser.cc"
%define parser_class_name (go_parser)
%define api.token.constructor
%define api.value.type variant
%define parse.assert
%define parser.error verbose
%locations

%code requires
{
#include <algorithm>
#include <iostream>
#include <list>
#include <map>
#include
}

%token	END 0
%token	IMPORT "import" PACKAGE "package" IDENTIFIER STRINGLITERAL
%%

program:		package
|				package imports;


package:		"package" STRINGLITERAL;

import:			"import" importitem
|				"import" '(' importitems ')';

imports:		imports import
|				import;

importitem:		STRINGLITERAL
|				IDENTIFIER STRINGLITERAL;

importitems:	importitems importitem
|				importitem;
